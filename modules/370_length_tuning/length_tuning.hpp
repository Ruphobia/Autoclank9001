// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <vector>

// Length tuning / meander insertion.
//
// Given a straight track between two endpoints and a target length in
// millimeters, replace the track with a zigzag polyline whose total
// path length matches the target within a tolerance. Useful for
// matched-length nets (DDR, high-speed clocks, ADC/DAC).
//
// The generated meander respects a bounding rectangle so it does not
// wander off the board or into other components.
namespace length_tuning {

struct MeanderOptions {
    double target_length_mm      = 0.0;
    double amplitude_mm          = 1.5;
    double period_mm             = 3.0;
    double corner_radius_mm      = 0.0;   // sharp corners for MVP; radius > 0 emits chamfered corners
    // Rectangular envelope inside which the meander must fit (world mm).
    // Zero-width -> unbounded.
    double envelope_min_x_mm = 0, envelope_max_x_mm = 0;
    double envelope_min_y_mm = 0, envelope_max_y_mm = 0;
};

struct MeanderResult {
    std::vector<geom::VECTOR2I> path;    // start, ..., end
    double achieved_length_mm = 0.0;
    bool ok = false;
    std::string reason;
};

// Compute a meandered path between `a` and `b` targeting the length.
// If target is <= direct distance, returns a straight line.
MeanderResult compute(geom::VECTOR2I a, geom::VECTOR2I b, const MeanderOptions & opts);

// Apply the meander to a Board: remove the existing track with `uuid`,
// insert the polyline as a chain of PcbTrack segments on the same net
// and layer.
struct ApplyResult {
    std::size_t segments_added = 0;
    double      achieved_length_mm = 0.0;
    bool        ok = false;
    std::string reason;
};

ApplyResult apply(kicad_model::Board & board, const kicad_model::UUID & track_uuid,
                  const MeanderOptions & opts);

}
