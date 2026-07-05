// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <unordered_map>

// Pick-and-place per-part overrides.
//
// The default pos-file emitter (352_fab_writers::write_pos_csv) uses
// the raw footprint (at, angle, placement_layer) values. Some
// manufacturers require different rotation values ("EP rotation") or
// side flips for specific components; this module holds those
// overrides as a table keyed by reference designator and applies them
// to a POS output.
namespace pos_overrides {

struct Override {
    double rotation_delta_deg = 0.0;   // added to the natural rotation
    bool   flip_side          = false; // swap F.Cu <-> B.Cu for the pos row
    double dx_mm              = 0.0;
    double dy_mm              = 0.0;
};

using Table = std::unordered_map<std::string, Override>;

// Apply overrides to a Board's footprints and return the CSV.
std::string write_pos_csv(const kicad_model::Board & board, const Table & overrides);

// JSON round-trip for storing the override table alongside the design.
std::string to_json  (const Table & t);
Table       from_json(std::string_view text);

}
