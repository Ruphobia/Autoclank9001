// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

// Native writers for fabrication output. Bypasses kicad-cli; consumes
// the in-memory Board model directly.
//
// Gerber output follows RS-274X + Gerber X2 attributes (the modern
// format all fab houses accept). One file per copper layer plus soldermask,
// silk, paste, edge cuts on request.
//
// Excellon drill output follows the KiCad convention: one file for PTH
// drills, optional file for NPTH.
namespace fab_writers {

struct GerberOptions {
    // Trailing / leading zero suppression per Ucamco spec.
    // We use "trailing zero suppression" by default; matches KiCad and
    // is universally accepted by fab houses.
    bool suppress_trailing = true;
    // Precision: 3.5 (nm) means 3 digits before the decimal, 5 after.
    // 4.6 is the modern default (for boards up to 100 m at nm-precise).
    int integer_digits = 4;
    int decimal_digits = 6;
    // Include X2 attributes.
    bool include_x2 = true;
    // Comment header lines to prepend (verbatim).
    std::vector<std::string> header_comments;
};

// Produce a gerber string for a single copper / silk / mask layer.
// `layer_name` uses the KiCad canonical form ("F.Cu", "B.SilkS", ...).
std::string write_gerber_layer(const kicad_model::Board & board,
                               std::string_view layer_name,
                               const GerberOptions & opts = {});

struct DrillOptions {
    // Excellon: metric, ",0000" (4 decimal places) is the KiCad default.
    int decimal_digits = 4;
    // Emit an ODB++-compatible header.
    bool metric = true;
    // Slot vs round holes: emit slots via G85 (route) blocks when true.
    bool emit_slots = true;
    // Suppress zeros: "trailing" is universal.
    bool suppress_trailing = true;
};

// Produce an Excellon drill file covering every plated-through-hole
// (PTH) on the board plus vias.
std::string write_drill_pth(const kicad_model::Board & board, const DrillOptions & opts = {});
// Same for np_thru_hole pads (mounting holes, keepouts).
std::string write_drill_npth(const kicad_model::Board & board, const DrillOptions & opts = {});

// Emit a `.gbrjob` job file (JSON) listing the produced gerbers plus
// stackup info. Fab houses read this to know which file is which layer.
std::string write_job_file(const kicad_model::Board & board,
                           const std::unordered_map<std::string, std::string> & layer_files,
                           const std::string & drill_file = "");

// Emit a pick-and-place CSV.
std::string write_pos_csv(const kicad_model::Board & board);

} // namespace fab_writers
