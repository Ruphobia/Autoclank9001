// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <vector>

// Fast, approximate DRC for interactive feedback.
//
// Full DRC (modules/351_drc) walks every pair of items; that's fine
// for a "run at end" pass but too slow to call on every mouse-move
// while drawing a track.
//
// This module does a single-item vs board check: given a proposed new
// item (a track segment being drawn or a via being placed), it returns
// the list of collisions using coarse bounding-radius math. Runtime is
// O(items), typically sub-millisecond on real boards.
//
// The result set can drive a live "red highlight" overlay in the
// canvas or gate the mouse-click that would commit the item.
namespace fast_drc {

struct Collision {
    std::string other_uuid;
    std::string reason;         // "clearance", "hole_to_hole", ...
    double      clearance_mm;   // computed clearance (negative = overlap)
    geom::VECTOR2I at;          // representative point (midpoint)
};

// Proposed item under construction.
struct ProposedTrack {
    geom::VECTOR2I start{0, 0};
    geom::VECTOR2I end  {0, 0};
    long long      width_nm = 0;
    std::string    layer    = "F.Cu";
    int            net      = 0;
};

struct ProposedVia {
    geom::VECTOR2I at{0, 0};
    long long      size_nm  = 0;
    long long      drill_nm = 0;
    int            net      = 0;
    std::vector<std::string> layers;
};

struct Config {
    double base_clearance_mm  = 0.15;
    double base_hole_hole_mm  = 0.25;
};

std::vector<Collision> check_track(const kicad_model::Board & board,
                                    const ProposedTrack & prop,
                                    const Config & cfg = {});

std::vector<Collision> check_via  (const kicad_model::Board & board,
                                    const ProposedVia & prop,
                                    const Config & cfg = {});

}
