// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>

// Differential-pair routing helper.
//
// Given two endpoints per net (positive + negative), route both nets
// as a parallel pair maintaining constant gap. Simplified MVP:
//   * Both nets must start on the same layer.
//   * The two positive endpoints and the two negative endpoints are
//     assumed to be at symmetric positions (as they'd be after a diff
//     pair placement pass).
//   * Route uses a two-segment L-shape offset perpendicular by
//     (width + gap).
//
// Handles the common case of a straight diff-pair run between two
// footprints; skew tuning and coupled length matching are follow-ups.
namespace diff_pair {

struct Config {
    long long track_width_nm = 200000;   // 0.2 mm
    long long gap_nm         = 250000;   // 0.25 mm (edge-to-edge)
    std::string layer        = "F.Cu";
    int p_net  = 0;
    int n_net  = 0;
};

struct Result {
    std::size_t p_segments = 0;
    std::size_t n_segments = 0;
    bool ok = false;
    std::string reason;
};

Result route(kicad_model::Board & board,
             geom::VECTOR2I p_start, geom::VECTOR2I p_end,
             geom::VECTOR2I n_start, geom::VECTOR2I n_end,
             const Config & cfg);

}
