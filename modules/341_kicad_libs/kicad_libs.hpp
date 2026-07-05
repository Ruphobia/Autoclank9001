// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Symbol / footprint / 3D-model library index for the KiCad integration.
// MVP scope: enumerate .kicad_sym and .pretty locations and expose linear
// name search. Follow-on work builds a SQLite FTS5 index per
// scratchpad/kicad_integration.md §7.
namespace kicad_libs {

struct SymbolHit {
    std::string lib;          // e.g. "Timer"
    std::string name;         // e.g. "NE555"
    std::string description;  // may be empty
    std::string source_path;  // .kicad_sym file this came from
};

struct FootprintHit {
    std::string lib;          // e.g. "Package_DIP"
    std::string name;         // e.g. "DIP-8_W7.62mm"
    std::string description;  // may be empty
    std::string source_path;  // .kicad_mod file this came from
    std::int32_t pad_count = 0;
    bool         smd       = false;
};

struct Config {
    std::string symbol_root;      // root dir containing *.kicad_sym libs
    std::string footprint_root;   // root dir containing *.pretty/*.kicad_mod
    std::string package3d_root;   // root dir containing *.3dshapes/*.step | *.wrl
    std::size_t symbol_libs      = 0;
    std::size_t footprint_libs   = 0;
    bool        ready            = false;
};

void init();
void shutdown();
const Config & config();

// Case-insensitive substring match on symbol name and (when available)
// description. `limit` caps the returned vector.
std::vector<SymbolHit>    search_symbols   (std::string_view query, std::size_t limit = 20);
std::vector<FootprintHit> search_footprints(std::string_view query, std::size_t limit = 20);

// Exact-match lookup by "lib:name". Returns the first hit or an empty
// value.
SymbolHit    find_symbol   (std::string_view lib_id);
FootprintHit find_footprint(std::string_view lib_id);

// Slice the .kicad_sym file and return the full text of the
// (symbol "lib_id" ...) top-level block. Returns empty on miss. Used by
// 843_schematic_capture to inline lib_symbols in generated schematics.
std::string extract_symbol_block   (std::string_view lib_id);
// Slice the .kicad_mod file and return its entire content (footprints
// are always one-file-one-footprint in kicad_mod).
std::string extract_footprint_block(std::string_view lib_id);

}
