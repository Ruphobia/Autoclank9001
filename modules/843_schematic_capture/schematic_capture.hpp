// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../343_circuit_intent/circuit_intent.hpp"

#include <string>
#include <string_view>
#include <vector>

// Emit a KiCad `.kicad_sch` schematic from a circuit_intent::Intent.
//
// MVP scope:
//   * inline the referenced lib_symbols from the local KiCad symbol
//     library (via 341_kicad_libs), so the file stands alone
//   * place each SCH_SYMBOL instance at a grid position
//   * emit a text block listing every intended net (documentation)
//   * DO NOT emit wire segments yet; connectivity is expressed in the
//     .kicad_pcb NETINFO block instead (see 844_pcb_layout). ERC on the
//     produced schematic will therefore warn about unconnected pins;
//     the .kicad_pro emitter suppresses that class into 'ignore' when
//     the tool-generated project is being used.
//
// Follow-up work:
//   * global-label pass at pin endpoints (needs pin (at ...) extraction)
//   * orthogonal wire routing between grouped parts
//   * multi-sheet hierarchical schematics
namespace schematic_capture {

struct Options {
    // A4 landscape, all mm.
    double page_width_mm  = 297.0;
    double page_height_mm = 210.0;

    // Distance between placed symbols.
    double grid_pitch_mm  = 25.4;

    // Top-left origin for the first placed symbol.
    double origin_x_mm    = 20.0;
    double origin_y_mm    = 30.0;

    // Columns before wrapping to a new row.
    int    columns        = 5;

    // Generator string embedded in the file.
    std::string generator          = "tool";
    std::string generator_version  = "0.1";
};

struct Result {
    // The full .kicad_sch text. Empty on failure.
    std::string sch_text;

    // Diagnostics from the emitter (missing lib symbols, resolved
    // substitutions, etc.). Distinct from intent validation which
    // happens upstream.
    std::vector<circuit_intent::Diagnostic> diagnostics;

    // The parts that were successfully resolved (their lib_symbol
    // block was found in the library index).
    int resolved_parts = 0;
    // Parts that had no lib_symbol found and got a placeholder block.
    int placeholder_parts = 0;

    bool ok = false;
};

void init();
void shutdown();

// Legacy status API preserved for ac9_test compatibility.
struct Status {
    bool        ready = false;
    std::string detail;
};
Status status();

// Primary entry point. `intent` should already have passed
// circuit_intent::validate() with no Errors; the emitter still runs on
// warnings-only input but the diagnostics are surfaced.
Result from_intent(const circuit_intent::Intent & intent,
                   const Options & opts = {});

// Convenience: write the sch_text to disk. Returns true on success.
bool write_file(const std::string & path, const Result & r);

}
