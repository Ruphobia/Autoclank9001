// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <vector>

// Simplified push-and-shove router.
//
// This is not a re-implementation of KiCad's PNS (which is 38k LoC of
// research-grade code). The MVP algorithm here:
//   1. Try to add the new track. Compute all colliding tracks and vias
//      (via 365_fast_drc) on the same layer.
//   2. For each collider, compute a perpendicular displacement vector
//      that would restore the required clearance.
//   3. If the displacement can be applied without cascading into
//      unsolvable violations (recursive check with depth cap), apply it.
//   4. If any collider cannot be shoved, the operation fails (the
//      caller reverts and the user gets a "cannot route" signal).
//
// The engine handles the two everyday cases well: routing a new track
// through a channel with obstacle tracks that can slide out of the
// way, and dragging a segment across a board.
namespace pns_shove {

struct Config {
    double clearance_mm  = 0.15;
    int    max_depth     = 4;
    // Bounding box beyond which shoves are refused (protects against
    // pushing tracks off the board).
    double shove_range_mm = 30.0;
};

struct ShoveOp {
    kicad_model::UUID uuid;
    long long         dx_nm = 0;
    long long         dy_nm = 0;
};

struct Result {
    bool                  ok = false;
    std::string           reason;
    std::vector<ShoveOp>  applied_shoves;
    std::size_t           new_track_index = 0;   // board.items index of the new track (on success)
};

// Attempt to add a new track. If ok=false, no state is changed.
// If ok=true, `applied_shoves` lists every track that got moved.
Result add_track(kicad_model::Board & board,
                 const kicad_model::PcbTrack & prop,
                 const Config & cfg = {});

// Drag an existing track: try to move it by (dx, dy). Same shove
// behavior for obstacles that get in the way.
Result drag_track(kicad_model::Board & board,
                  const kicad_model::UUID & uuid,
                  long long dx_nm, long long dy_nm,
                  const Config & cfg = {});

}
