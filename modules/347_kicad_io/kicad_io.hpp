// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../344_sexpr/sexpr.hpp"
#include "../346_kicad_model/kicad_model.hpp"

#include <optional>
#include <string>
#include <string_view>

// Read and write KiCad's native s-expression file formats using our
// sexpr parser and kicad_model data model. Round-trip is the contract:
// read_sch(x) -> write_sch(x) opens cleanly in KiCad; read/write of the
// same file is idempotent modulo cosmetic whitespace.
namespace kicad_io {

struct IOError {
    std::string message;
    std::size_t line   = 0;
    std::size_t column = 0;
    std::size_t offset = 0;
};

// -------------------- .kicad_sch --------------------

// Parse text -> Schematic. On failure returns std::nullopt and fills
// `err` when non-null.
std::optional<kicad_model::Schematic>
    read_schematic(std::string_view text, IOError * err = nullptr);
std::optional<kicad_model::Schematic>
    read_schematic_file(std::string_view path, IOError * err = nullptr);

// Serialize back to text.
std::string write_schematic(const kicad_model::Schematic & sch);
bool        write_schematic_file(std::string_view path,
                                 const kicad_model::Schematic & sch);

// -------------------- .kicad_pcb --------------------

std::optional<kicad_model::Board>
    read_board(std::string_view text, IOError * err = nullptr);
std::optional<kicad_model::Board>
    read_board_file(std::string_view path, IOError * err = nullptr);

std::string write_board(const kicad_model::Board & board);
bool        write_board_file(std::string_view path,
                             const kicad_model::Board & board);

// -------------------- .kicad_sym and .kicad_mod ------

// Symbol library: a file containing zero or more (symbol ...) top-level
// entries. Returned as a name->LibSymbol map keyed by the fully
// qualified lib_id when present, else the bare name.
std::optional<std::unordered_map<std::string, kicad_model::LibSymbol>>
    read_symbol_library(std::string_view text, IOError * err = nullptr);
std::string write_symbol_library(
    const std::unordered_map<std::string, kicad_model::LibSymbol> & lib,
    const std::string & lib_name = "");

// Footprint file: one (footprint ...) per file. Returned as a
// Footprint model item (or nullopt on failure).
std::optional<kicad_model::Footprint>
    read_footprint(std::string_view text, IOError * err = nullptr);
std::string write_footprint(const kicad_model::Footprint & fp);

} // namespace kicad_io
