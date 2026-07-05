// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../343_circuit_intent/circuit_intent.hpp"

#include <string>
#include <string_view>
#include <vector>

// Emit a KiCad `.kicad_pcb` board file from a circuit_intent::Intent.
//
// MVP scope:
//   * emit the required (setup) block with fab-safe default rules
//   * emit the full (layers) block for a 2-layer board
//   * place each footprint at a grid position
//   * inline every referenced footprint definition from the local
//     kicad-footprints (via 341_kicad_libs); placeholder when missing
//   * emit (net N "name") entries for every net in the intent
//   * emit board outline on Edge.Cuts for the configured board size
//   * DO NOT emit routed traces yet; the ratsnest is derived by KiCad
//     at load time
//
// Follow-up: place-and-route pass (freerouting subprocess or in-process
// PNS wrap), zone fills for power planes, teardrops, courtyard checks.
namespace pcb_layout {

struct Options {
    // Board outline. Overrides intent.board.* when non-zero.
    double board_width_mm  = 0.0;
    double board_height_mm = 0.0;

    // Grid pitch for footprint placement.
    double grid_pitch_mm   = 12.5;
    double origin_x_mm     = 15.0;
    double origin_y_mm     = 15.0;
    int    columns         = 4;

    std::string generator          = "ac9";
    std::string generator_version  = "0.1";

    // Fab-safe defaults (JLCPCB standard).
    double default_track_width_mm  = 0.2;
    double default_clearance_mm    = 0.15;
    double default_via_diameter_mm = 0.6;
    double default_via_drill_mm    = 0.3;
};

struct Result {
    std::string pcb_text;
    std::vector<circuit_intent::Diagnostic> diagnostics;
    int resolved_footprints    = 0;
    int placeholder_footprints = 0;
    int emitted_nets           = 0;
    bool ok = false;
};

void init();
void shutdown();

struct Status {
    bool        ready = false;
    std::string detail;
};
Status status();

Result from_intent(const circuit_intent::Intent & intent,
                   const Options & opts = {});

bool write_file(const std::string & path, const Result & r);

}
