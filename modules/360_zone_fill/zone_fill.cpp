// SPDX-License-Identifier: GPL-3.0-or-later
#include "zone_fill.hpp"

#include <algorithm>
#include <cmath>

namespace zone_fill {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::Pad;
using kicad_model::Zone;
using kicad_model::ItemType;
using geom::VECTOR2I;
using geom::SHAPE_POLY_SET;
using geom::SHAPE_LINE_CHAIN;

namespace {

VECTOR2I world_pad(const Footprint & fp, const Pad & p) {
    double a = fp.angle.rad();
    double c = std::cos(a), s = std::sin(a);
    long long lx = p.at.x, ly = p.at.y;
    return { fp.at.x + static_cast<long long>(std::llround(lx * c - ly * s)),
             fp.at.y + static_cast<long long>(std::llround(lx * s + ly * c)) };
}

bool pad_on_layer(const Pad & p, std::string_view layer) {
    for (const auto & L : p.layers) {
        if (L == layer) return true;
        if (L == "*.Cu" && (layer == "F.Cu" || layer == "B.Cu")) return true;
    }
    return false;
}

// Build an axis-aligned rectangle chain around center with (w, h) half-extents.
SHAPE_LINE_CHAIN aabb_chain(VECTOR2I center, long long half_w, long long half_h) {
    SHAPE_LINE_CHAIN c;
    c.append({ center.x - half_w, center.y - half_h });
    c.append({ center.x + half_w, center.y - half_h });
    c.append({ center.x + half_w, center.y + half_h });
    c.append({ center.x - half_w, center.y + half_h });
    c.set_closed(true);
    return c;
}

} // namespace

Report fill_all(Board & board, const Options & opts) {
    Report r;
    long long extra_nm = geom::mm_to_nm(opts.extra_clearance_mm);

    // Collect footprints once.
    std::vector<const Footprint*> fps;
    for (const auto & it : board.items) {
        if (it->type == ItemType::PcbFootprint)
            fps.push_back(static_cast<const Footprint*>(it.get()));
    }

    for (auto & it : board.items) {
        if (it->type != ItemType::PcbZone) continue;
        auto * z = static_cast<Zone*>(it.get());
        z->filled_polys.clear();
        if (z->polys.empty()) continue;

        SHAPE_POLY_SET filled;
        filled.add_outline(z->polys[0]);

        long long total_clearance = z->clearance_nm + extra_nm;
        std::string zone_layer = z->layers.empty() ? std::string("F.Cu") : z->layers[0];

        for (const auto * fp : fps) {
            for (const auto & p : fp->pads) {
                if (!pad_on_layer(p, zone_layer)) continue;
                if (p.net == z->net && z->net > 0) continue;   // same-net -> keep (thermal deferred)
                VECTOR2I center = world_pad(*fp, p);
                long long hw = p.size.x / 2 + total_clearance;
                long long hh = p.size.y / 2 + total_clearance;
                filled.add_hole(0, aabb_chain(center, hw, hh));
                ++r.obstacles_carved;
            }
        }
        z->filled_polys.push_back(std::move(filled));
        ++r.zones_processed;
    }
    return r;
}

} // namespace zone_fill
