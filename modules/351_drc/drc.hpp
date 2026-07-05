// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <vector>

// DRC (Design Rules Check) engine.
//
// Rules covered in this pass:
//   * clearance: pad<->pad, pad<->track, track<->track (same layer,
//     different net, ignoring items that share a net)
//   * hole_to_hole clearance for drilled pads and vias
//   * copper_edge_clearance: copper items too close to Edge.Cuts
//   * courtyards_overlap: F.CrtYd/B.CrtYd polygons that overlap
//   * missing_courtyard: footprint with no CrtYd graphics
//   * unconnected_items: pads/traces on the "" (id 0) net when a
//     ratsnest connection is missing
//
// Not covered in this pass (follow-up):
//   * annular_width (pad drill vs pad size sanity)
//   * silk_over_copper, silk_overlap, silk_edge_clearance
//   * zone_has_empty_net, zones_intersect
//   * copper_sliver, isolated_copper
//   * differential-pair-specific rules
//
// Uses circle/segment/box distance approximations from 345_geom;
// pad shapes other than circle and rectangle are treated as their
// axis-aligned bounding box, which is safe (over-approximates
// clearance violations only, never under-approximates).
namespace drc {

enum class Severity : int { Error = 0, Warning = 1, Info = 2, Ignore = 3 };

struct Violation {
    std::string rule_id;
    Severity    severity = Severity::Error;
    std::string message;
    bool           has_pos = false;
    geom::VECTOR2I at{0,0};
    std::string     layer;
};

struct Report {
    std::vector<Violation> violations;
    std::size_t errors   = 0;
    std::size_t warnings = 0;
    std::size_t infos    = 0;
    std::size_t ignored  = 0;
};

struct Config {
    // Base clearance in mm when the board has no netclass defaults on
    // the item pair. Overridden per-netclass in a future pass.
    double base_clearance_mm     = 0.15;
    double base_hole_hole_mm     = 0.25;
    double copper_edge_mm        = 0.3;

    Severity clearance           = Severity::Error;
    Severity hole_to_hole        = Severity::Error;
    Severity copper_edge_clearance = Severity::Error;
    Severity courtyards_overlap  = Severity::Error;
    Severity missing_courtyard   = Severity::Warning;
    Severity unconnected_items   = Severity::Error;
    Severity annular_width       = Severity::Warning;
    Severity silk_over_copper    = Severity::Warning;
    double   min_annular_mm      = 0.1;
};

Report run(const kicad_model::Board & board, const Config & cfg = {});

// Emit JSON envelope compatible with kicad-cli pcb drc --format json.
std::string to_kicad_json(const Report & rep, std::string_view source_path);

} // namespace drc
