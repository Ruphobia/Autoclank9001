// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../345_geom/geom.hpp"

#include <string>
#include <string_view>
#include <vector>

// KiCad drawing-sheet ("page layout") templates: title blocks, borders,
// custom text with expandable variables (${TITLE}, ${DATE}, etc).
// File extension: .kicad_wks (s-expression).
//
// MVP scope: read + write a subset of ds items (line, rect, text,
// polygon, bitmap-ref) enough for a functional custom title block.
namespace pagelayout {

struct DsText {
    std::string     text;             // may contain ${VAR}
    geom::VECTOR2I  at{0,0};
    geom::EDA_ANGLE angle{0};
    double          font_h_mm = 1.5;
    double          font_v_mm = 1.5;
    double          thickness_mm = 0.15;
    bool            bold = false;
    bool            italic = false;
    std::string     justify;
    std::string     name;             // internal name / label
};

struct DsLine {
    geom::VECTOR2I  start{0,0};
    geom::VECTOR2I  end{0,0};
    double          width_mm = 0.15;
};

struct DsRect {
    geom::VECTOR2I  start{0,0};
    geom::VECTOR2I  end{0,0};
    double          width_mm = 0.15;
};

struct DrawingSheet {
    std::string setup_name = "custom";
    // Page size + margins in mm.
    double page_width_mm  = 297.0;
    double page_height_mm = 210.0;
    double left_margin_mm   = 10.0;
    double right_margin_mm  = 10.0;
    double top_margin_mm    = 10.0;
    double bottom_margin_mm = 10.0;

    std::vector<DsText> texts;
    std::vector<DsLine> lines;
    std::vector<DsRect> rects;
};

DrawingSheet read (std::string_view text);
std::string  write(const DrawingSheet & ds);

// Expand ${VAR} placeholders using a lookup map (title/date/rev/...).
std::string expand(std::string_view raw,
                   const std::vector<std::pair<std::string, std::string>> & vars);

// Default KiCad-style A4 title block. Useful starting point for the
// pagelayout editor tab.
DrawingSheet default_a4_titleblock();

}
