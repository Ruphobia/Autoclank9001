// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../345_geom/geom.hpp"

#include <string>
#include <string_view>
#include <vector>

// Native gerber (RS-274X) parser + SVG rasterizer.
// The viewer we ship in the frontend currently loads SVGs produced by
// kicad-cli. This module bypasses that: parse the .gbr text directly
// and emit an SVG the browser can display without KiCad installed.
// Covers the subset of RS-274X we produce ourselves in 352_fab_writers:
//   * FS (format spec), MO (units), LP (polarity)
//   * ADD aperture defs (circle, rect, oval)
//   * D01 (draw), D02 (move), D03 (flash)
//   * G01 (linear), G02/G03 (arc), G75 (multi-quadrant), G36/G37 (region)
namespace gerber_viewer_native {

struct Options {
    // Stroke/fill for the SVG output.
    std::string stroke_color = "#c88a17";
    std::string fill_color   = "#c88a17";
    // Padding around content in mm.
    double pad_mm = 2.0;
};

struct Parsed {
    std::vector<std::pair<geom::VECTOR2I, geom::VECTOR2I>> lines;    // draws
    std::vector<std::pair<geom::VECTOR2I, long long>>       flashes;  // (pos, aperture_dia_nm)
    geom::BOX2I bbox;
    std::string warnings;
    bool ok = false;
};

Parsed  parse(std::string_view text);
std::string to_svg(const Parsed & p, const Options & opts = {});

// One-shot helper: parse then emit.
std::string render_to_svg(std::string_view gerber_text, const Options & opts = {});

}
