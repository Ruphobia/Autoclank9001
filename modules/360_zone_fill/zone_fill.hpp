// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>

// Zone fill for copper pours.
//
// Full Clipper2 boolean ops are a follow-up; this MVP produces
// filled_polys via a simple obstacle-carve approach:
//   * For each Zone in the board, start with the zone's outline
//     polygon as the filled region.
//   * For each Pad on the same layer with a different net (or net 0),
//     carve a rectangular hole equal to the pad's AABB, inflated by
//     the zone clearance.
//   * Same-net pads are left alone (thermal relief is a follow-up).
//
// This produces an approximate solid pour that renders correctly for
// most casual designs and passes DRC when clearances are set high
// enough. Real polygon booleans come when Clipper2 lands.
namespace zone_fill {

struct Options {
    // Extra clearance mm to inflate obstacles by (in addition to the
    // zone's own clearance_nm field).
    double extra_clearance_mm = 0.0;
};

struct Report {
    std::size_t zones_processed  = 0;
    std::size_t obstacles_carved = 0;
    std::string warnings;
};

// Compute Zone::filled_polys for every zone on the board. Overwrites
// any previous fills. Idempotent.
Report fill_all(kicad_model::Board & board, const Options & opts = {});

}
