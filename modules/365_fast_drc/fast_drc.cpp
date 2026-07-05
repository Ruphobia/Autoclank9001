// SPDX-License-Identifier: GPL-3.0-or-later
#include "fast_drc.hpp"

#include <algorithm>
#include <cmath>

namespace fast_drc {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::Pad;
using kicad_model::PcbTrack;
using kicad_model::PcbVia;
using kicad_model::ItemType;
using geom::VECTOR2I;
using geom::nm_to_mm;

namespace {

VECTOR2I world_pad(const Footprint & fp, const Pad & p) {
    double a = fp.angle.rad();
    double c = std::cos(a), s = std::sin(a);
    long long lx = p.at.x, ly = p.at.y;
    return { fp.at.x + static_cast<long long>(std::llround(lx * c - ly * s)),
             fp.at.y + static_cast<long long>(std::llround(lx * s + ly * c)) };
}

bool pad_on_layer(const Pad & p, const std::string & layer) {
    for (const auto & L : p.layers) {
        if (L == layer) return true;
        if (L == "*.Cu" && (layer == "F.Cu" || layer == "B.Cu")) return true;
    }
    return false;
}

long long pad_effective_radius_nm(const Pad & p) {
    return std::max(p.size.x, p.size.y) / 2;
}

} // namespace

std::vector<Collision> check_track(const Board & board, const ProposedTrack & pt, const Config & cfg) {
    std::vector<Collision> out;
    geom::SEG seg{ pt.start, pt.end };

    // 1. Against every pad on the same layer.
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        for (const auto & pad : fp->pads) {
            if (pad.net == pt.net && pt.net > 0) continue;
            if (!pad_on_layer(pad, pt.layer)) continue;
            VECTOR2I c = world_pad(*fp, pad);
            double dist  = seg.distance(c);
            double clear = nm_to_mm(static_cast<long long>(dist)
                                    - pt.width_nm / 2
                                    - pad_effective_radius_nm(pad));
            if (clear < cfg.base_clearance_mm) {
                Collision co;
                co.other_uuid = pad.uuid.empty() ? fp->uuid : pad.uuid;
                co.reason     = "clearance";
                co.clearance_mm = clear;
                co.at         = c;
                out.push_back(std::move(co));
            }
        }
    }

    // 2. Against every other track on the same layer.
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbTrack) continue;
        const auto * t = static_cast<const PcbTrack*>(it.get());
        if (t->layer != pt.layer) continue;
        if (t->net == pt.net && pt.net > 0) continue;
        geom::SEG other{ t->start, t->end };
        double d;
        auto ix = seg.intersect(other);
        if (ix) d = 0;
        else    d = std::min({ seg.distance(t->start),
                                seg.distance(t->end),
                                other.distance(pt.start),
                                other.distance(pt.end) });
        double clear = nm_to_mm(static_cast<long long>(d) - pt.width_nm/2 - t->width_nm/2);
        if (clear < cfg.base_clearance_mm) {
            Collision co;
            co.other_uuid = t->uuid;
            co.reason     = "clearance";
            co.clearance_mm = clear;
            co.at = { (pt.start.x + pt.end.x)/2, (pt.start.y + pt.end.y)/2 };
            out.push_back(std::move(co));
        }
    }

    // 3. Against vias.
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbVia) continue;
        const auto * v = static_cast<const PcbVia*>(it.get());
        // Via is present on every layer between top and bottom of its layer span;
        // approximate as "always considered".
        if (v->net == pt.net && pt.net > 0) continue;
        double dist = seg.distance(v->at);
        double clear = nm_to_mm(static_cast<long long>(dist) - pt.width_nm/2 - v->size_nm/2);
        if (clear < cfg.base_clearance_mm) {
            Collision co;
            co.other_uuid = v->uuid;
            co.reason     = "clearance";
            co.clearance_mm = clear;
            co.at = v->at;
            out.push_back(std::move(co));
        }
    }
    return out;
}

std::vector<Collision> check_via(const Board & board, const ProposedVia & pv, const Config & cfg) {
    std::vector<Collision> out;

    for (const auto & it : board.items) {
        if (it->type == ItemType::PcbFootprint) {
            const auto * fp = static_cast<const Footprint*>(it.get());
            for (const auto & pad : fp->pads) {
                if (pad.net == pv.net && pv.net > 0) continue;
                VECTOR2I c = world_pad(*fp, pad);
                double d  = std::hypot(static_cast<double>(pv.at.x - c.x),
                                        static_cast<double>(pv.at.y - c.y));
                double clear = nm_to_mm(static_cast<long long>(d)
                                        - pv.size_nm/2
                                        - pad_effective_radius_nm(pad));
                if (clear < cfg.base_clearance_mm) {
                    Collision co;
                    co.other_uuid = pad.uuid.empty() ? fp->uuid : pad.uuid;
                    co.reason     = "clearance";
                    co.clearance_mm = clear;
                    co.at         = c;
                    out.push_back(std::move(co));
                }
                // Hole-to-hole for through-hole pads.
                if (pad.drill_nm > 0 && pv.drill_nm > 0) {
                    double hc = nm_to_mm(static_cast<long long>(d) - pad.drill_nm/2 - pv.drill_nm/2);
                    if (hc < cfg.base_hole_hole_mm) {
                        Collision co;
                        co.other_uuid   = pad.uuid.empty() ? fp->uuid : pad.uuid;
                        co.reason       = "hole_to_hole";
                        co.clearance_mm = hc;
                        co.at           = c;
                        out.push_back(std::move(co));
                    }
                }
            }
        } else if (it->type == ItemType::PcbVia) {
            const auto * v = static_cast<const PcbVia*>(it.get());
            if (v->net == pv.net && pv.net > 0) continue;
            double d = std::hypot(static_cast<double>(pv.at.x - v->at.x),
                                   static_cast<double>(pv.at.y - v->at.y));
            double clear = nm_to_mm(static_cast<long long>(d) - pv.size_nm/2 - v->size_nm/2);
            if (clear < cfg.base_clearance_mm) {
                Collision co;
                co.other_uuid = v->uuid;
                co.reason     = "clearance";
                co.clearance_mm = clear;
                co.at         = v->at;
                out.push_back(std::move(co));
            }
            double hc = nm_to_mm(static_cast<long long>(d) - pv.drill_nm/2 - v->drill_nm/2);
            if (hc < cfg.base_hole_hole_mm) {
                Collision co;
                co.other_uuid   = v->uuid;
                co.reason       = "hole_to_hole";
                co.clearance_mm = hc;
                co.at           = v->at;
                out.push_back(std::move(co));
            }
        }
    }
    return out;
}

} // namespace fast_drc
