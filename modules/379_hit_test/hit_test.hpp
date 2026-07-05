// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <vector>

// Server-side hit-test.
//
// Client passes a world point (mm) and an optional filter; the server
// returns every item whose bounding box contains that point. Used
// for rubber-band selection, click-to-select, and the AI pipeline's
// "what's under the cursor" query.
namespace hit_test {

struct Hit {
    kicad_model::UUID uuid;
    std::string kind;   // "SchSymbol"|"PcbFootprint"|"PcbTrack"|...
    double bbox_mm[4];  // {lo_x, lo_y, hi_x, hi_y}
};

std::vector<Hit> pick_sch(const kicad_model::Schematic & sch,
                          double x_mm, double y_mm, double radius_mm = 1.0);

std::vector<Hit> pick_pcb(const kicad_model::Board & board,
                          double x_mm, double y_mm, double radius_mm = 1.0);

// Box selection: every item whose bbox intersects the rectangle.
std::vector<Hit> select_sch(const kicad_model::Schematic & sch,
                            double lo_x_mm, double lo_y_mm,
                            double hi_x_mm, double hi_y_mm);
std::vector<Hit> select_pcb(const kicad_model::Board & board,
                            double lo_x_mm, double lo_y_mm,
                            double hi_x_mm, double hi_y_mm);

}
