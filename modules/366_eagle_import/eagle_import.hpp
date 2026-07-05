// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <optional>
#include <string>
#include <string_view>

// EAGLE (.brd, .sch) importer.
//
// EAGLE files are XML. This module targets the modern (6.x+) DTD, in
// which the top-level element is <eagle version="..."> containing
// <drawing><board>...</board></drawing> for .brd files and
// <drawing><schematic>...</schematic></drawing> for .sch files.
//
// MVP scope on .brd:
//   * Board outline (Dimension wires on layer 20 -> Edge.Cuts gr_lines)
//   * Elements (footprints): position, angle, package name
//   * Element pads (holes) and SMD pads
//   * Signals (nets) and their pin references
//   * Wires (tracks) on copper layers 1 (Top) / 16 (Bottom)
//   * Vias
//
// Not covered in MVP (follow-ups):
//   * Polygons / zones (needs shape parsing)
//   * Text on silk / user layers (basic pass done; no font styling)
//   * Custom padstacks
//   * Package graphics (silk/fab); footprints emit an empty
//     raw_graphics_sexpr and the layout still validates as long as
//     the corresponding .kicad_mod exists in the target lib.
namespace eagle_import {

struct ImportOptions {
    // How to map EAGLE layers to KiCad layers. Follows KiCad's
    // "Import Non-KiCad Board" defaults.
    // 1 -> F.Cu, 16 -> B.Cu, 20 -> Edge.Cuts, 21 -> F.SilkS,
    // 22 -> B.SilkS, 25 -> F.Fab, 26 -> B.Fab, 51 -> F.CrtYd,
    // 52 -> B.CrtYd, 27/28 -> Dwgs.User.
    // Callers can override by seeding remap[eagle_layer] = kicad_name.
    // Empty for defaults.
    // Not currently exposed as JSON; MVP uses hard-coded defaults.
};

struct ImportReport {
    std::size_t elements    = 0;
    std::size_t signals     = 0;
    std::size_t wires       = 0;
    std::size_t vias        = 0;
    std::size_t edge_cuts   = 0;
    std::string warnings;
    bool ok = false;
};

// Read an EAGLE .brd XML text into a Board model.
std::optional<kicad_model::Board> read_board(std::string_view xml, ImportReport * report = nullptr);

std::optional<kicad_model::Board> read_board_file(std::string_view path, ImportReport * report = nullptr);

}
