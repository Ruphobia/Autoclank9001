// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../345_geom/geom.hpp"

#include <string>
#include <string_view>
#include <vector>

// Native Excellon drill file viewer.
// Parses the standard KiCad-flavored .drl format that 352_fab_writers
// emits: METRIC/TZ header, T-code tool table, X/Y coordinates.
// Produces an SVG the browser can display.
namespace drill_viewer {

struct Hole {
    geom::VECTOR2I at;
    long long      dia_nm = 0;
};

struct Parsed {
    std::vector<Hole> holes;
    geom::BOX2I       bbox;
    std::string       warnings;
    bool metric = true;
    bool ok = false;
};

Parsed parse(std::string_view text);

struct SvgOptions {
    std::string hole_color = "#eaeaea";
    std::string bg_color   = "#131518";
    double pad_mm = 2.0;
};

std::string to_svg(const Parsed & p, const SvgOptions & opts = {});
std::string render_to_svg(std::string_view text, const SvgOptions & opts = {});

}
