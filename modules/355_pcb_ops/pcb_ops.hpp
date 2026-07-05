// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <vector>

// Backend PCB operations that don't fit into the tools JS layer:
//   * teardrop generator (drop-shaped copper at track->pad interfaces)
//   * alignment / distribution across selected items
//   * cross-probe helpers (sch<->pcb ref -> item uuid lookup)
namespace pcb_ops {

// Emit teardrop polygons around the pad/via ends of every track. Adds
// gr_polygon items on the same layer. Returns the number of teardrops
// created.
struct TeardropOptions {
    double length_ratio_of_width = 3.0;   // teardrop length as multiple of track width
    double width_ratio_of_track  = 1.2;   // width at pad end
    bool   include_vias          = true;
    bool   include_pads          = true;
};
std::size_t generate_teardrops(kicad_model::Board & board,
                               const TeardropOptions & opts = {});

// Alignment axis.
enum class AlignAxis { LeftX, RightX, CenterX, TopY, BottomY, CenterY };

// Align every item in `uuids` on `axis`. Uses the bounding box of
// each item's origin; footprints use fp->at as the reference point.
void align(kicad_model::Board & board, const std::vector<std::string> & uuids, AlignAxis axis);

// Distribute uniformly on X or Y.
void distribute(kicad_model::Board & board,
                const std::vector<std::string> & uuids, bool horizontal);

// Cross-probe: given a schematic reference (e.g. "R1"), return the PCB
// footprint UUIDs that match by Reference field.
std::vector<std::string> pcb_uuids_for_ref(const kicad_model::Board & board,
                                            std::string_view reference);

// Cross-probe the other way: schematic side.
std::vector<std::string> sch_uuids_for_ref(const kicad_model::Schematic & sch,
                                            std::string_view reference);

}
