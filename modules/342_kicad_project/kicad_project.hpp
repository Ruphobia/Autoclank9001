// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Emits a `.kicad_pro` JSON project file. The project file drives every
// downstream compute path: DRC severities, ERC severities, netclass
// widths, layer setup. Without a sane project file kicad-cli reverts to
// permissive built-in defaults and fabricators will reject the gerbers.
//
// See scratchpad/kicad_integration.md and the "kicad project file" chat
// dive for the section-by-section justification.
namespace kicad_project {

// A named electrical class. The .kicad_pro persists these on
// net_settings.classes; nets get bound via netclass_patterns or
// per-net netclass_assignments.
struct NetClass {
    std::string name;                    // "Default", "Power", "USB_HS"
    double      clearance_mm      = 0.2; // trace-to-trace min
    double      track_width_mm    = 0.2; // default trace width
    double      via_diameter_mm   = 0.6;
    double      via_drill_mm      = 0.3;
    double      uvia_diameter_mm  = 0.3;
    double      uvia_drill_mm     = 0.1;
    double      diff_pair_width_mm       = 0.2;
    double      diff_pair_gap_mm         = 0.25;
    double      diff_pair_via_gap_mm     = 0.25;
    // Regex/glob patterns of net names this class matches. Empty = manual assignment.
    std::vector<std::string> patterns;
};

// A pre-baked design-rule preset. The MVP ships JLCPCB standard-2-layer
// values; add more (JLCPCB 4-layer, PCBWay, OSHPark) as we grow.
struct FabProfile {
    std::string name;                 // "jlcpcb_default"
    double      trace_width_mil = 6;
    double      clearance_mil   = 6;
    double      via_drill_mm    = 0.3;
    double      via_diameter_mm = 0.6;
    double      board_edge_clearance_mm = 0.3;
};

// Whole project descriptor. Convert from a circuit_intent::Intent via
// from_intent() and serialize with to_json().
struct Project {
    std::string title;
    int         schema_version = 3;
    int         copper_layers  = 2;

    // Design rules
    std::vector<NetClass> netclasses;   // classes[0] must be "Default"
    FabProfile            fab_profile;

    // Severities, per KiCad's own defaults for anything we don't override.
    // Empty map = "use KiCad defaults", which is fine for the MVP.
    // Follow-up: expose fields for the specific ones tool cares about.
};

// Load one of the built-in profiles. Falls back to jlcpcb_default when
// `name` is unknown.
FabProfile builtin_profile(std::string_view name);

// Convenience: default 2-layer project matched to the JLCPCB standard
// fab profile. This is what get emitted when no override is given.
Project default_project(std::string_view title = "tool_generated");

// Emit the JSON blob. `indent` <=0 for a compact form.
std::string to_json(const Project & in, int indent = 2);

// The "runtime local" file that KiCad co-persists next to .kicad_pro.
// tool commits neither; we still generate a minimal one so KiCad opens
// the project without complaining on first launch.
std::string to_prl_json(const Project & in);

}
