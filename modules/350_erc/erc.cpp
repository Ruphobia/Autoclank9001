// SPDX-License-Identifier: GPL-3.0-or-later
#include "erc.hpp"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace erc {

namespace {

using kicad_model::Schematic;
using kicad_model::SchSymbol;
using kicad_model::SchLabel;
using kicad_model::SchGlobalLabel;
using kicad_model::SchHierLabel;
using kicad_model::SchWire;
using kicad_model::ItemType;

// KiCad's default 12x12 pin-connection matrix. Indexed [a][b] where
// a and b are PinType values.
// 0 = ok, 1 = warning, 2 = error.
const int PIN_MAP[12][12] = {
    // in  out  bidi tri  pas  uns  pin  pout oc   oe   nc   uncn
    {  0,   0,   0,   0,   0,   0,   0,   2,   0,   0,   0,   0}, // input
    {  0,   2,   0,   1,   0,   0,   0,   2,   2,   2,   2,   0}, // output
    {  0,   0,   0,   0,   0,   0,   0,   2,   1,   1,   2,   0}, // bidi
    {  0,   1,   0,   0,   0,   0,   0,   2,   1,   1,   2,   0}, // tri
    {  0,   0,   0,   0,   0,   0,   0,   2,   0,   0,   2,   0}, // passive
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   0}, // unspecified
    {  0,   0,   0,   0,   0,   0,   0,   2,   0,   0,   2,   0}, // power_in
    {  2,   2,   2,   2,   2,   0,   2,   2,   2,   2,   2,   0}, // power_out
    {  0,   2,   1,   1,   0,   0,   0,   2,   1,   2,   2,   0}, // open_collector
    {  0,   2,   1,   1,   0,   0,   0,   2,   2,   1,   2,   0}, // open_emitter
    {  0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   0}, // no_connect
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}  // unconnected
};

std::string sev_str(Severity s) {
    switch (s) {
        case Severity::Error:   return "error";
        case Severity::Warning: return "warning";
        case Severity::Info:    return "info";
        case Severity::Ignore:  return "ignore";
    }
    return "info";
}

void tally(Report & r, Violation v) {
    switch (v.severity) {
        case Severity::Error:   ++r.errors;   break;
        case Severity::Warning: ++r.warnings; break;
        case Severity::Info:    ++r.infos;    break;
        case Severity::Ignore:  ++r.ignored;  break;
    }
    r.violations.push_back(std::move(v));
}

} // namespace

PinType classify_pin(std::string_view e) {
    if (e == "input")             return PIN_INPUT;
    if (e == "output")            return PIN_OUTPUT;
    if (e == "bidirectional")     return PIN_BIDIRECTIONAL;
    if (e == "tri_state")         return PIN_TRISTATE;
    if (e == "passive")           return PIN_PASSIVE;
    if (e == "unspecified")       return PIN_UNSPECIFIED;
    if (e == "power_in")          return PIN_POWER_IN;
    if (e == "power_out")         return PIN_POWER_OUT;
    if (e == "open_collector")    return PIN_OPEN_COLLECTOR;
    if (e == "open_emitter")      return PIN_OPEN_EMITTER;
    if (e == "no_connect" ||
        e == "not_connected")     return PIN_NO_CONNECT;
    return PIN_UNSPECIFIED;
}

int pin_compatibility(PinType a, PinType b) {
    int ai = std::clamp(static_cast<int>(a), 0, 11);
    int bi = std::clamp(static_cast<int>(b), 0, 11);
    return PIN_MAP[ai][bi];
}

Report run(const Schematic & sch, const netlist::Netlist & nl, const Config & cfg) {
    Report r;

    // 1. Duplicate reference designators + missing Reference / Value.
    std::unordered_map<std::string, std::vector<geom::VECTOR2I>> refs;
    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        const auto * s = static_cast<const SchSymbol *>(it.get());
        std::string ref = s->reference();
        std::string val = s->value();
        if (ref.empty()) {
            Violation v;
            v.rule_id  = "missing_reference";
            v.severity = cfg.missing_reference;
            v.message  = "symbol has no reference (lib_id " + s->lib_id + ")";
            v.has_pos  = true;
            v.at       = s->at;
            tally(r, std::move(v));
        } else {
            refs[ref].push_back(s->at);
        }
        if (val.empty()) {
            Violation v;
            v.rule_id  = "missing_value";
            v.severity = cfg.missing_value;
            v.message  = "symbol " + ref + " has no value";
            v.has_pos  = true;
            v.at       = s->at;
            v.component_refs = {ref};
            tally(r, std::move(v));
        }

        if (sch.lib_symbols.find(s->lib_id) == sch.lib_symbols.end()) {
            Violation v;
            v.rule_id  = "unresolved_lib_symbol";
            v.severity = cfg.unresolved_lib_symbol;
            v.message  = "lib_symbol not found for " + s->lib_id;
            v.has_pos  = true;
            v.at       = s->at;
            v.component_refs = {ref};
            tally(r, std::move(v));
        }
    }
    for (const auto & kv : refs) {
        if (kv.second.size() > 1) {
            Violation v;
            v.rule_id  = "duplicate_reference";
            v.severity = cfg.duplicate_reference;
            v.message  = "reference designator '" + kv.first + "' used " +
                         std::to_string(kv.second.size()) + " times";
            v.has_pos  = true;
            v.at       = kv.second.front();
            v.component_refs = {kv.first};
            tally(r, std::move(v));
        }
    }

    // 2. Per-net checks.
    for (const auto & n : nl.nets) {
        if (n.id == 0) continue; // no-connect bin

        // 2a. Pin not connected: exactly one pin, no explicit no_connect.
        if (n.pins.size() == 1) {
            const auto & p = n.pins.front();
            Violation v;
            v.rule_id  = "pin_not_connected";
            v.severity = cfg.pin_not_connected;
            v.message  = "pin " + p.ref + "." + p.number + " (" + p.electrical +
                         ") not connected to any other pin";
            v.component_refs = {p.ref};
            v.net_name       = n.name;
            tally(r, std::move(v));
        }

        // 2b. Pin-to-pin compatibility.
        for (std::size_t i = 0; i < n.pins.size(); ++i) {
            for (std::size_t j = i + 1; j < n.pins.size(); ++j) {
                auto ta = classify_pin(n.pins[i].electrical);
                auto tb = classify_pin(n.pins[j].electrical);
                int  cc = pin_compatibility(ta, tb);
                if (cc == 0) continue;
                Violation v;
                v.rule_id  = "pin_to_pin";
                v.severity = (cc == 2) ? cfg.pin_to_pin // errors sometimes downgraded
                                       : Severity::Warning;
                // KiCad exposes both severities; keep the config value.
                v.severity = cfg.pin_to_pin;
                v.message  = "pin conflict on net '" + n.name + "': " +
                             n.pins[i].ref + "." + n.pins[i].number + " (" + n.pins[i].electrical + ") " +
                             "vs " +
                             n.pins[j].ref + "." + n.pins[j].number + " (" + n.pins[j].electrical + ")";
                v.component_refs = { n.pins[i].ref, n.pins[j].ref };
                v.net_name       = n.name;
                tally(r, std::move(v));
            }
        }

        // 2c. Pin not driven.
        bool has_input = false, has_driver = false;
        bool has_power_in = false, has_power_out = false;
        for (const auto & p : n.pins) {
            auto t = classify_pin(p.electrical);
            if (t == PIN_INPUT || t == PIN_BIDIRECTIONAL) has_input = true;
            if (t == PIN_OUTPUT || t == PIN_TRISTATE || t == PIN_BIDIRECTIONAL ||
                t == PIN_POWER_OUT || t == PIN_OPEN_COLLECTOR || t == PIN_OPEN_EMITTER)
                has_driver = true;
            if (t == PIN_POWER_IN)  has_power_in  = true;
            if (t == PIN_POWER_OUT) has_power_out = true;
        }
        if (has_input && !has_driver && !has_power_in) {
            Violation v;
            v.rule_id  = "pin_not_driven";
            v.severity = cfg.pin_not_driven;
            v.message  = "net '" + n.name + "' has input pins but no driver";
            v.net_name = n.name;
            for (const auto & p : n.pins) v.component_refs.push_back(p.ref);
            tally(r, std::move(v));
        }
        if (has_power_in && !has_power_out) {
            Violation v;
            v.rule_id  = "power_pin_not_driven";
            v.severity = cfg.power_pin_not_driven;
            v.message  = "net '" + n.name + "' has a power_in pin but no power_out driver";
            v.net_name = n.name;
            for (const auto & p : n.pins) v.component_refs.push_back(p.ref);
            tally(r, std::move(v));
        }
    }

    // 3. Dangling labels. Label at (x,y) with no wire endpoint or pin.
    std::unordered_set<long long> connected_keys; // pack (x/1000)<<32 | (y/1000)
    auto pack = [](geom::VECTOR2I v) -> long long {
        long long xs = v.x / 1000, ys = v.y / 1000;
        return (xs & 0xFFFFFFFFLL) << 32 | (ys & 0xFFFFFFFFLL);
    };
    // Collect pin positions.
    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        const auto * s = static_cast<const SchSymbol *>(it.get());
        auto lib_it = sch.lib_symbols.find(s->lib_id);
        if (lib_it == sch.lib_symbols.end()) continue;
        for (const auto & p : lib_it->second.pins) {
            long long lx = p.at.x, ly = p.at.y;
            if (s->mirror_x) ly = -ly;
            if (s->mirror_y) lx = -lx;
            double a = s->angle.rad();
            double c = std::cos(a), sn = std::sin(a);
            long long wx = s->at.x + static_cast<long long>(std::llround(lx * c - ly * sn));
            long long wy = s->at.y + static_cast<long long>(std::llround(lx * sn + ly * c));
            connected_keys.insert(pack({wx, wy}));
        }
    }
    // Collect wire endpoints.
    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchWire) continue;
        const auto * w = static_cast<const SchWire *>(it.get());
        for (const auto & p : w->pts) connected_keys.insert(pack(p));
    }
    // Check labels.
    auto check_label = [&](const SchLabel * l, const char * kind) {
        if (!connected_keys.count(pack(l->at))) {
            Violation v;
            v.rule_id  = "dangling_label";
            v.severity = cfg.dangling_label;
            v.message  = std::string(kind) + " label '" + l->text + "' is not attached to any pin or wire";
            v.has_pos  = true;
            v.at       = l->at;
            tally(r, std::move(v));
        }
    };
    for (const auto & it : sch.root.items) {
        if      (it->type == ItemType::SchLabel)       check_label(static_cast<const SchLabel*>(it.get()),       "local");
        else if (it->type == ItemType::SchGlobalLabel) check_label(static_cast<const SchGlobalLabel*>(it.get()), "global");
        else if (it->type == ItemType::SchHierLabel)   check_label(static_cast<const SchHierLabel*>(it.get()),   "hierarchical");
    }

    return r;
}

std::string to_kicad_json(const Report & rep, std::string_view source_path) {
    std::ostringstream os;
    os << "{\n"
       << "  \"$schema\": \"kicad_erc\",\n"
       << "  \"source\": \"" << source_path << "\",\n"
       << "  \"kicad_version\": \"tool/erc\",\n"
       << "  \"coordinate_units\": \"mm\",\n"
       << "  \"violations\": [\n";
    for (std::size_t i = 0; i < rep.violations.size(); ++i) {
        const auto & v = rep.violations[i];
        os << "    {\n"
           << "      \"type\": \""     << v.rule_id  << "\",\n"
           << "      \"severity\": \"" << sev_str(v.severity) << "\",\n"
           << "      \"description\": \"" << v.message << "\",\n"
           << "      \"items\": [";
        if (v.has_pos) {
            os << "{\"description\":\"\",\"pos\":{\"x\":"
               << geom::nm_to_mm(v.at.x) << ",\"y\":"
               << geom::nm_to_mm(v.at.y) << "}}";
        }
        os << "]\n"
           << "    }" << (i + 1 < rep.violations.size() ? "," : "") << "\n";
    }
    os << "  ]\n}\n";
    return os.str();
}

} // namespace erc
