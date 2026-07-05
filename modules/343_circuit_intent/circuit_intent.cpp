// SPDX-License-Identifier: GPL-3.0-or-later
#include "circuit_intent.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <regex>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace circuit_intent {
namespace {

using json = nlohmann::json;

std::string get_str(const json & j, const char * k, const char * dflt = "") {
    if (!j.is_object()) return dflt;
    auto it = j.find(k);
    if (it == j.end() || !it->is_string()) return dflt;
    return it->get<std::string>();
}

double get_num(const json & j, const char * k, double dflt = 0.0) {
    if (!j.is_object()) return dflt;
    auto it = j.find(k);
    if (it == j.end() || !it->is_number()) return dflt;
    return it->get<double>();
}

int get_int(const json & j, const char * k, int dflt = 0) {
    if (!j.is_object()) return dflt;
    auto it = j.find(k);
    if (it == j.end() || !it->is_number_integer()) return dflt;
    return it->get<int>();
}

bool get_bool(const json & j, const char * k, bool dflt = false) {
    if (!j.is_object()) return dflt;
    auto it = j.find(k);
    if (it == j.end() || !it->is_boolean()) return dflt;
    return it->get<bool>();
}

std::string strip(std::string_view s) {
    std::size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return std::string(s.substr(a, b - a));
}

} // namespace

bool split_endpoint(std::string_view ep, std::string & ref_out, std::string & pin_out) {
    auto dot = ep.find('.');
    if (dot == std::string_view::npos) return false;
    ref_out.assign(ep.substr(0, dot));
    pin_out.assign(ep.substr(dot + 1));
    return !ref_out.empty() && !pin_out.empty();
}

bool parse_json(std::string_view json_text, Intent & out, std::string & error_out) {
    out = {};
    json j = json::parse(json_text, nullptr, /*allow_exceptions=*/false);
    if (j.is_discarded()) {
        error_out = "malformed JSON";
        return false;
    }
    if (!j.is_object()) {
        error_out = "top-level must be an object";
        return false;
    }

    // meta
    if (j.contains("meta") && j["meta"].is_object()) {
        out.title = get_str(j["meta"], "title");
        out.notes = get_str(j["meta"], "notes");
    } else {
        out.title = get_str(j, "title");
        out.notes = get_str(j, "notes");
    }

    // power_nets
    if (j.contains("power") && j["power"].is_object() &&
        j["power"].contains("nets") && j["power"]["nets"].is_array()) {
        for (const auto & pn : j["power"]["nets"]) {
            PowerNet n;
            n.name    = get_str(pn, "name");
            n.voltage = get_num(pn, "voltage");
            if (!n.name.empty()) out.power_nets.push_back(std::move(n));
        }
    }

    // parts
    if (j.contains("parts") && j["parts"].is_array()) {
        for (const auto & pj : j["parts"]) {
            Part p;
            p.ref             = get_str(pj, "ref");
            p.value           = get_str(pj, "value");
            p.lib_hint        = get_str(pj, "lib_hint");
            p.footprint_hint  = get_str(pj, "footprint_hint");
            p.mpn             = get_str(pj, "mpn");
            p.manufacturer    = get_str(pj, "manufacturer");
            p.datasheet_url   = get_str(pj, "datasheet_url");
            p.dnp             = get_bool(pj, "dnp");
            p.lib_hint_locked = get_bool(pj, "lib_hint_locked");
            if (!p.ref.empty()) out.parts.push_back(std::move(p));
        }
    }

    // connections: two supported shapes.
    //   (a) "connections": [["U1.VCC", "+9V"], ...]  -> edge form
    //   (b) "nets": [{"name":"+9V","endpoints":["U1.VCC","R1.1"]}, ...]  -> net form
    if (j.contains("nets") && j["nets"].is_array()) {
        for (const auto & nj : j["nets"]) {
            Net n;
            n.name     = get_str(nj, "name");
            n.netclass = get_str(nj, "netclass", "Default");
            if (nj.contains("endpoints") && nj["endpoints"].is_array()) {
                for (const auto & ep : nj["endpoints"]) {
                    if (ep.is_string()) n.endpoints.push_back({ep.get<std::string>()});
                }
            }
            out.nets.push_back(std::move(n));
        }
    }
    if (j.contains("connections") && j["connections"].is_array()) {
        // Convert (a) to (b) using a union-find over endpoint labels.
        // Each connected component becomes one Net. Named endpoints (like
        // "+9V") preserve the net name; otherwise the net stays unnamed.
        std::unordered_map<std::string, std::string> parent;
        std::function<std::string(const std::string &)> root =
            [&](const std::string & x) -> std::string {
                auto it = parent.find(x);
                if (it == parent.end() || it->second == x) return x;
                std::string r = root(it->second);
                parent[x] = r;
                return r;
            };
        auto unite = [&](const std::string & a, const std::string & b) {
            parent.emplace(a, a);
            parent.emplace(b, b);
            std::string ra = root(a), rb = root(b);
            if (ra != rb) parent[ra] = rb;
        };
        for (const auto & pair : j["connections"]) {
            if (!pair.is_array() || pair.size() != 2) continue;
            if (!pair[0].is_string() || !pair[1].is_string()) continue;
            unite(pair[0].get<std::string>(), pair[1].get<std::string>());
        }
        std::unordered_map<std::string, Net> by_root;
        for (const auto & kv : parent) {
            const std::string & x = kv.first;
            std::string r = root(x);
            auto & n = by_root[r];
            n.endpoints.push_back({x});
            // Named net = an endpoint without a dot.
            if (x.find('.') == std::string::npos && n.name.empty()) {
                n.name = x;
            }
        }
        for (auto & kv : by_root) {
            if (kv.second.netclass.empty()) kv.second.netclass = "Default";
            out.nets.push_back(std::move(kv.second));
        }
    }

    // board
    if (j.contains("placement_hints") && j["placement_hints"].is_object()) {
        const auto & ph = j["placement_hints"];
        if (ph.contains("board") && ph["board"].is_object()) {
            const auto & b = ph["board"];
            if (b.contains("size_mm") && b["size_mm"].is_array() && b["size_mm"].size() == 2) {
                out.board.width_mm  = b["size_mm"][0].get<double>();
                out.board.height_mm = b["size_mm"][1].get<double>();
            }
            out.board.copper_layers = get_int(b, "layers", 2);
            out.board.fab_profile   = get_str(b, "fab_profile", "jlcpcb_default");
        }
        if (ph.contains("group") && ph["group"].is_array()) {
            for (const auto & gj : ph["group"]) {
                PlacementGroup g;
                g.near = get_str(gj, "near");
                if (gj.contains("refs") && gj["refs"].is_array()) {
                    for (const auto & r : gj["refs"]) {
                        if (r.is_string()) g.refs.push_back(r.get<std::string>());
                    }
                }
                if (!g.refs.empty()) out.placement_hints.push_back(std::move(g));
            }
        }
    }

    return true;
}

std::string to_json(const Intent & in, int indent) {
    json j = json::object();
    j["meta"] = { {"title", in.title}, {"notes", in.notes} };

    json pn = json::array();
    for (const auto & n : in.power_nets) {
        pn.push_back({ {"name", n.name}, {"voltage", n.voltage} });
    }
    j["power"] = { {"nets", pn} };

    json parts = json::array();
    for (const auto & p : in.parts) {
        json pj = {
            {"ref", p.ref},
            {"value", p.value},
            {"lib_hint", p.lib_hint},
            {"footprint_hint", p.footprint_hint}
        };
        if (!p.mpn.empty())            pj["mpn"] = p.mpn;
        if (!p.manufacturer.empty())   pj["manufacturer"] = p.manufacturer;
        if (!p.datasheet_url.empty())  pj["datasheet_url"] = p.datasheet_url;
        if (p.dnp)                     pj["dnp"] = true;
        if (p.lib_hint_locked)         pj["lib_hint_locked"] = true;
        parts.push_back(std::move(pj));
    }
    j["parts"] = std::move(parts);

    json nets = json::array();
    for (const auto & n : in.nets) {
        json nj = { {"name", n.name}, {"netclass", n.netclass} };
        json eps = json::array();
        for (const auto & e : n.endpoints) eps.push_back(e.endpoint);
        nj["endpoints"] = std::move(eps);
        nets.push_back(std::move(nj));
    }
    j["nets"] = std::move(nets);

    json ph = json::object();
    ph["board"] = {
        {"size_mm", json::array({in.board.width_mm, in.board.height_mm})},
        {"layers",  in.board.copper_layers},
        {"fab_profile", in.board.fab_profile}
    };
    json groups = json::array();
    for (const auto & g : in.placement_hints) {
        json gj = { {"near", g.near}, {"refs", json::array()} };
        for (const auto & r : g.refs) gj["refs"].push_back(r);
        groups.push_back(std::move(gj));
    }
    ph["group"] = std::move(groups);
    j["placement_hints"] = std::move(ph);

    return j.dump(indent);
}

std::vector<Diagnostic> validate(const Intent & in) {
    std::vector<Diagnostic> out;
    static const std::regex ref_pat("^[A-Z]+[0-9]+$");

    // Unique ref-designators; ref shape.
    std::unordered_set<std::string> refs;
    for (std::size_t i = 0; i < in.parts.size(); ++i) {
        const auto & p = in.parts[i];
        std::ostringstream field; field << "parts[" << i << "].ref";
        if (p.ref.empty()) {
            out.push_back({Diagnostic::Severity::Error, field.str(), "ref-designator empty"});
            continue;
        }
        if (!std::regex_match(p.ref, ref_pat)) {
            out.push_back({Diagnostic::Severity::Warning, field.str(),
                           "ref-designator '" + p.ref + "' does not match [A-Z]+[0-9]+"});
        }
        if (!refs.insert(p.ref).second) {
            out.push_back({Diagnostic::Severity::Error, field.str(),
                           "duplicate ref-designator '" + p.ref + "'"});
        }
        if (p.value.empty()) {
            std::ostringstream vf; vf << "parts[" << i << "].value";
            out.push_back({Diagnostic::Severity::Warning, vf.str(),
                           "part '" + p.ref + "' has empty value"});
        }
    }

    // Named nets referenced but not in power_nets are still valid: they
    // become local named nets in the schematic. Just track which are used.
    std::unordered_set<std::string> known_named_nets;
    for (const auto & pn : in.power_nets) known_named_nets.insert(pn.name);

    for (std::size_t i = 0; i < in.nets.size(); ++i) {
        const auto & n = in.nets[i];
        for (std::size_t k = 0; k < n.endpoints.size(); ++k) {
            const auto & ep = n.endpoints[k].endpoint;
            std::ostringstream field;
            field << "nets[" << i << "].endpoints[" << k << "]";
            if (ep.empty()) {
                out.push_back({Diagnostic::Severity::Error, field.str(), "empty endpoint"});
                continue;
            }
            std::string ref, pin;
            if (split_endpoint(ep, ref, pin)) {
                if (refs.find(ref) == refs.end()) {
                    out.push_back({Diagnostic::Severity::Error, field.str(),
                                   "endpoint '" + ep + "' references unknown ref '" + ref + "'"});
                }
            } else {
                known_named_nets.insert(ep);
            }
        }
        if (n.endpoints.size() < 2) {
            std::ostringstream field; field << "nets[" << i << "]";
            out.push_back({Diagnostic::Severity::Warning, field.str(),
                           "net has fewer than two endpoints"});
        }
    }

    // Board dims positive and finite.
    auto bad = [](double x) { return !(std::isfinite(x) && x > 0.0); };
    if (bad(in.board.width_mm) || bad(in.board.height_mm)) {
        out.push_back({Diagnostic::Severity::Error, "placement_hints.board.size_mm",
                       "board dimensions must be positive finite"});
    }
    if (in.board.copper_layers < 1 || in.board.copper_layers > 32
        || (in.board.copper_layers % 2 != 0 && in.board.copper_layers != 1)) {
        out.push_back({Diagnostic::Severity::Warning, "placement_hints.board.layers",
                       "copper_layers should be 1, 2, 4, 6, 8, ..."});
    }

    return out;
}

std::vector<Edge> pair_edges(const Intent & in) {
    std::vector<Edge> out;
    for (const auto & n : in.nets) {
        for (std::size_t i = 0; i + 1 < n.endpoints.size(); ++i) {
            for (std::size_t j = i + 1; j < n.endpoints.size(); ++j) {
                out.push_back({n.endpoints[i].endpoint,
                               n.endpoints[j].endpoint,
                               n.name});
            }
        }
    }
    return out;
}

} // namespace circuit_intent
