// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

// Parsers for the JSON output of `kicad-cli sch erc` and
// `kicad-cli pcb drc`. Both commands emit the same envelope shape:
//   { "$schema": "...", "source": "...", "date": "...",
//     "kicad_version": "...", "coordinate_units": "mm",
//     "violations": [ {
//        "type": "clearance",
//        "severity": "error",
//        "description": "Clearance violation...",
//        "items": [ { "uuid":"...", "description":"...", "pos": {"x": .., "y": ..} }, ... ]
//     }, ... ],
//     "unconnected_items": [...] }   (DRC only)
namespace kicad_bridge {

struct Violation {
    std::string type;         // rule identifier (e.g. "clearance", "pin_not_connected")
    std::string severity;     // "error" | "warning" | "info" | "exclusion"
    std::string description;  // free-text summary
    // Optional geometry: at most one (x,y) per violation for the first
    // affected item. Used by the web viewer to pan/zoom to hits.
    bool        has_pos = false;
    double      x_mm    = 0.0;
    double      y_mm    = 0.0;
    // Deep items array serialized as pretty JSON for the trail (debug).
    std::string items_json;
};

struct Report {
    std::string             source;             // .kicad_sch or .kicad_pcb path
    std::string             kicad_version;
    std::string             coordinate_units;
    std::vector<Violation>  violations;
    // Populated for DRC only.
    std::vector<Violation>  unconnected_items;
    std::size_t             errors   = 0;
    std::size_t             warnings = 0;
    std::size_t             ignored  = 0;
    bool                    ok       = false;   // parse succeeded
    std::string             parse_error;
};

// Parse the raw JSON text produced by `kicad-cli sch erc --format json`
// or `kicad-cli pcb drc --format json`.
Report parse_report(std::string_view json_text);

// Read the file first.
Report load_report(std::string_view path);

// Compact human summary: "3 errors, 2 warnings across 5 rule types."
std::string summarize(const Report & r);

}
