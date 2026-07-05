// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <optional>
#include <string>
#include <string_view>

// Altium / Protel ASCII (.PCB) importer.
//
// The modern binary Altium formats (.PcbDoc / .SchDoc / .PrjPcb) are
// documented only under NDA and require substantial reverse-engineering
// to read. The ASCII "PCB" format used by older Protel and by the
// "PCB4.0 ASCII" export from newer Altium is however text-based and
// well-covered by community documentation.
//
// This module reads the ASCII form:
//
//   PCB FILE VERSION 3.00
//   [record type] tag=value tag=value ...
//   [record type] ...
//
// Records covered: TRACK, PAD (as isolated pad NPTH/PTH), VIA,
// COMPONENT (component instance), FILL (copper rectangle).
namespace altium_import {

struct ImportReport {
    std::size_t tracks = 0, pads = 0, vias = 0, components = 0, fills = 0;
    std::string warnings;
    bool ok = false;
};

std::optional<kicad_model::Board> read_board(std::string_view text, ImportReport * rep = nullptr);

}
