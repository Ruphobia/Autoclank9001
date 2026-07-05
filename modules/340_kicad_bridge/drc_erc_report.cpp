// SPDX-License-Identifier: GPL-3.0-or-later
#include "drc_erc_report.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace kicad_bridge {
namespace {

using json = nlohmann::json;

std::string jstr(const json & j, const char * k) {
    if (!j.is_object()) return {};
    auto it = j.find(k);
    return (it != j.end() && it->is_string()) ? it->get<std::string>() : std::string{};
}

// Convert a violation JSON node to our compact form.
Violation to_violation(const json & j) {
    Violation v;
    v.type        = jstr(j, "type");
    v.severity    = jstr(j, "severity");
    v.description = jstr(j, "description");
    if (j.contains("items") && j["items"].is_array() && !j["items"].empty()) {
        const auto & first = j["items"][0];
        if (first.is_object() && first.contains("pos") && first["pos"].is_object()) {
            const auto & pos = first["pos"];
            if (pos.contains("x") && pos["x"].is_number() &&
                pos.contains("y") && pos["y"].is_number()) {
                v.has_pos = true;
                v.x_mm    = pos["x"].get<double>();
                v.y_mm    = pos["y"].get<double>();
            }
        }
        v.items_json = j["items"].dump(2);
    }
    return v;
}

void tally(std::vector<Violation> & bucket, std::size_t & err,
           std::size_t & warn, std::size_t & ign, const Violation & v) {
    bucket.push_back(v);
    if      (v.severity == "error")     ++err;
    else if (v.severity == "warning")   ++warn;
    else if (v.severity == "exclusion" || v.severity == "ignore") ++ign;
}

} // namespace

Report parse_report(std::string_view json_text) {
    Report r;
    if (json_text.empty()) {
        r.parse_error = "empty input";
        return r;
    }
    json j = json::parse(json_text, nullptr, /*allow_exceptions=*/false);
    if (j.is_discarded() || !j.is_object()) {
        r.parse_error = "not a JSON object";
        return r;
    }
    r.source            = jstr(j, "source");
    r.kicad_version     = jstr(j, "kicad_version");
    r.coordinate_units  = jstr(j, "coordinate_units");

    if (j.contains("violations") && j["violations"].is_array()) {
        for (const auto & vj : j["violations"]) {
            tally(r.violations, r.errors, r.warnings, r.ignored, to_violation(vj));
        }
    }
    if (j.contains("unconnected_items") && j["unconnected_items"].is_array()) {
        for (const auto & vj : j["unconnected_items"]) {
            tally(r.unconnected_items, r.errors, r.warnings, r.ignored, to_violation(vj));
        }
    }
    // Also report parity errors when present.
    if (j.contains("parity_errors") && j["parity_errors"].is_array()) {
        for (const auto & vj : j["parity_errors"]) {
            tally(r.violations, r.errors, r.warnings, r.ignored, to_violation(vj));
        }
    }
    r.ok = true;
    return r;
}

Report load_report(std::string_view path) {
    std::ifstream f{std::string(path)};
    if (!f) { Report r; r.parse_error = "cannot open " + std::string(path); return r; }
    std::stringstream ss; ss << f.rdbuf();
    return parse_report(ss.str());
}

std::string summarize(const Report & r) {
    if (!r.ok) return "parse failed: " + r.parse_error;
    std::unordered_map<std::string, std::size_t> by_type;
    auto count = [&](const std::vector<Violation> & v) {
        for (const auto & x : v) ++by_type[x.type];
    };
    count(r.violations);
    count(r.unconnected_items);
    std::ostringstream os;
    os << r.errors << " errors, " << r.warnings << " warnings across "
       << by_type.size() << " rule types";
    return os.str();
}

} // namespace kicad_bridge
