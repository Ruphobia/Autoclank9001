// SPDX-License-Identifier: GPL-3.0-or-later
#include "hit_test.hpp"

#include <algorithm>
#include <cmath>

namespace hit_test {

using kicad_model::Schematic;
using kicad_model::Board;
using kicad_model::ItemType;
using geom::nm_to_mm;

namespace {

Hit make_hit(const std::string & uuid, const char * kind,
             double lo_x, double lo_y, double hi_x, double hi_y) {
    Hit h;
    h.uuid = uuid;
    h.kind = kind;
    h.bbox_mm[0] = lo_x; h.bbox_mm[1] = lo_y;
    h.bbox_mm[2] = hi_x; h.bbox_mm[3] = hi_y;
    return h;
}

bool point_in_bbox(double x, double y, double lo_x, double lo_y, double hi_x, double hi_y) {
    return x >= lo_x && x <= hi_x && y >= lo_y && y <= hi_y;
}
bool bbox_intersects(double a_lo_x, double a_lo_y, double a_hi_x, double a_hi_y,
                     double b_lo_x, double b_lo_y, double b_hi_x, double b_hi_y) {
    return !(a_hi_x < b_lo_x || a_lo_x > b_hi_x || a_hi_y < b_lo_y || a_lo_y > b_hi_y);
}

using kicad_model::SchSymbol;
using kicad_model::SchWire;
using kicad_model::SchJunction;
using kicad_model::SchLabel;
using kicad_model::Footprint;
using kicad_model::PcbTrack;
using kicad_model::PcbVia;

Hit bbox_for_sch_item(const kicad_model::Item & it) {
    Hit h;
    switch (it.type) {
        case ItemType::SchSymbol: {
            const auto & s = static_cast<const SchSymbol&>(it);
            double cx = nm_to_mm(s.at.x), cy = nm_to_mm(s.at.y);
            return make_hit(s.uuid, "SchSymbol", cx - 5, cy - 5, cx + 5, cy + 5);
        }
        case ItemType::SchWire: {
            const auto & w = static_cast<const SchWire&>(it);
            double lo_x = 1e30, lo_y = 1e30, hi_x = -1e30, hi_y = -1e30;
            for (const auto & p : w.pts) {
                double x = nm_to_mm(p.x), y = nm_to_mm(p.y);
                lo_x = std::min(lo_x, x); hi_x = std::max(hi_x, x);
                lo_y = std::min(lo_y, y); hi_y = std::max(hi_y, y);
            }
            return make_hit(w.uuid, "SchWire", lo_x - 0.3, lo_y - 0.3, hi_x + 0.3, hi_y + 0.3);
        }
        case ItemType::SchJunction: {
            const auto & j = static_cast<const SchJunction&>(it);
            double x = nm_to_mm(j.at.x), y = nm_to_mm(j.at.y);
            return make_hit(j.uuid, "SchJunction", x - 0.5, y - 0.5, x + 0.5, y + 0.5);
        }
        case ItemType::SchLabel:
        case ItemType::SchGlobalLabel:
        case ItemType::SchHierLabel: {
            const auto & l = static_cast<const SchLabel&>(it);
            double x = nm_to_mm(l.at.x), y = nm_to_mm(l.at.y);
            double w = std::max<double>(2, static_cast<double>(l.text.size()) * 1.0);
            return make_hit(l.uuid, "SchLabel", x - 0.5, y - 1.5, x + w, y + 0.5);
        }
        default: break;
    }
    h.uuid = it.uuid; h.kind = "Unknown"; return h;
}

Hit bbox_for_pcb_item(const kicad_model::Item & it) {
    switch (it.type) {
        case ItemType::PcbFootprint: {
            const auto & f = static_cast<const Footprint&>(it);
            double cx = nm_to_mm(f.at.x), cy = nm_to_mm(f.at.y);
            return make_hit(f.uuid, "PcbFootprint", cx - 3, cy - 3, cx + 3, cy + 3);
        }
        case ItemType::PcbTrack: {
            const auto & t = static_cast<const PcbTrack&>(it);
            double lo_x = std::min(nm_to_mm(t.start.x), nm_to_mm(t.end.x));
            double hi_x = std::max(nm_to_mm(t.start.x), nm_to_mm(t.end.x));
            double lo_y = std::min(nm_to_mm(t.start.y), nm_to_mm(t.end.y));
            double hi_y = std::max(nm_to_mm(t.start.y), nm_to_mm(t.end.y));
            double w = nm_to_mm(t.width_nm) / 2;
            return make_hit(t.uuid, "PcbTrack", lo_x - w, lo_y - w, hi_x + w, hi_y + w);
        }
        case ItemType::PcbVia: {
            const auto & v = static_cast<const PcbVia&>(it);
            double cx = nm_to_mm(v.at.x), cy = nm_to_mm(v.at.y);
            double r  = nm_to_mm(v.size_nm) / 2;
            return make_hit(v.uuid, "PcbVia", cx - r, cy - r, cx + r, cy + r);
        }
        default: break;
    }
    return make_hit(it.uuid, "Unknown", 0, 0, 0, 0);
}

} // namespace

std::vector<Hit> pick_sch(const Schematic & sch, double x, double y, double radius) {
    std::vector<Hit> out;
    for (const auto & it : sch.root.items) {
        Hit h = bbox_for_sch_item(*it);
        if (point_in_bbox(x, y, h.bbox_mm[0] - radius, h.bbox_mm[1] - radius,
                                h.bbox_mm[2] + radius, h.bbox_mm[3] + radius))
            out.push_back(h);
    }
    return out;
}

std::vector<Hit> pick_pcb(const Board & board, double x, double y, double radius) {
    std::vector<Hit> out;
    for (const auto & it : board.items) {
        Hit h = bbox_for_pcb_item(*it);
        if (point_in_bbox(x, y, h.bbox_mm[0] - radius, h.bbox_mm[1] - radius,
                                h.bbox_mm[2] + radius, h.bbox_mm[3] + radius))
            out.push_back(h);
    }
    return out;
}

std::vector<Hit> select_sch(const Schematic & sch,
                            double lo_x, double lo_y, double hi_x, double hi_y) {
    std::vector<Hit> out;
    for (const auto & it : sch.root.items) {
        Hit h = bbox_for_sch_item(*it);
        if (bbox_intersects(lo_x, lo_y, hi_x, hi_y,
                            h.bbox_mm[0], h.bbox_mm[1], h.bbox_mm[2], h.bbox_mm[3]))
            out.push_back(h);
    }
    return out;
}

std::vector<Hit> select_pcb(const Board & board,
                            double lo_x, double lo_y, double hi_x, double hi_y) {
    std::vector<Hit> out;
    for (const auto & it : board.items) {
        Hit h = bbox_for_pcb_item(*it);
        if (bbox_intersects(lo_x, lo_y, hi_x, hi_y,
                            h.bbox_mm[0], h.bbox_mm[1], h.bbox_mm[2], h.bbox_mm[3]))
            out.push_back(h);
    }
    return out;
}

} // namespace hit_test
