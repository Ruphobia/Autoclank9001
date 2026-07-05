// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <vector>

// First-line-of-sight router.
//
// This is not a PNS router. It's a simple, single-net helper:
//   * for a given pair of endpoints, try a straight L-shape path
//     (horizontal-then-vertical or vertical-then-horizontal)
//   * if either candidate is clear (no clearance violations against
//     obstacles), commit tracks
//   * otherwise, skip and report "obstructed"
//
// It handles the "unobstructed nets" case that most beginner boards
// have and gives users a quick "auto-route what you can" pass. Full
// obstacle-avoiding routing (with shove or backtrack) is task 15a.
namespace line_router {

struct Config {
    double clearance_mm    = 0.15;
    long long track_width_nm = 200000; // 0.2 mm
    std::string layer      = "F.Cu";
};

struct SingleResult {
    bool routed = false;
    std::string reason;   // "clear" | "obstructed" | "same_point"
};

// Route two endpoints (a, b) as an L-shape on `cfg.layer` with the
// given width for net `net_id`. Adds PcbTracks to board on success.
SingleResult route_pair(kicad_model::Board & board, int net_id,
                        geom::VECTOR2I a, geom::VECTOR2I b,
                        const Config & cfg = {});

// Route every net in the board that is currently entirely disconnected
// (no PcbTrack items) using pad centers as endpoints. For nets with 3+
// pads, a simple MST-through-net-order approach is used.
struct BatchResult {
    std::size_t routed_pairs     = 0;
    std::size_t obstructed_pairs = 0;
    std::string log;
};
BatchResult route_all_unrouted(kicad_model::Board & board, const Config & cfg = {});

}
