// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <vector>

// Editable board-level setup: netclasses, layer stackup, physical
// thickness, design-rule constants. These are the fields that live in
// (setup ...) inside .kicad_pcb and the net_settings block of
// .kicad_pro. The MVP model preserves those blocks as raw
// s-expressions on read; this module gives structured accessors so
// the board setup dialog can edit them without hand-writing sexpr.
namespace board_setup {

struct NetClass {
    std::string name;
    double clearance_mm      = 0.15;
    double track_width_mm    = 0.2;
    double via_diameter_mm   = 0.6;
    double via_drill_mm      = 0.3;
    double uvia_diameter_mm  = 0.3;
    double uvia_drill_mm     = 0.1;
    double diff_pair_width_mm  = 0.2;
    double diff_pair_gap_mm    = 0.25;
    std::vector<std::string> patterns;
};

struct Stackup {
    std::vector<kicad_model::LayerInfo> layers;
    double board_thickness_mm = 1.6;
};

struct Setup {
    Stackup stackup;
    std::vector<NetClass> classes;   // classes[0] is always "Default"
    // Global design rules.
    double min_clearance_mm    = 0.15;
    double min_track_width_mm  = 0.15;
    double min_via_diameter_mm = 0.4;
    double min_via_drill_mm    = 0.2;
    double min_hole_clearance_mm = 0.25;
    double min_hole_to_hole_mm = 0.25;
    double min_copper_edge_clearance_mm = 0.3;
    bool   allow_blind_buried_vias = false;
    bool   allow_microvias         = false;
};

// Extract a Setup from a Board. Reads Board.raw_setup_sexpr (if set)
// and pulls out anything we recognize; falls back to defaults for
// unknown fields.
Setup extract(const kicad_model::Board & board);

// Merge a Setup back into a Board: rebuild raw_setup_sexpr and update
// board.layers + board.thickness_mm.
void apply(kicad_model::Board & board, const Setup & setup);

// JSON round-trip.
std::string to_json  (const Setup & s);
Setup       from_json(std::string_view text);

}
