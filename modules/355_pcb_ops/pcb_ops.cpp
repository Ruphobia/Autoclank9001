// SPDX-License-Identifier: GPL-3.0-or-later
#include "pcb_ops.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_set>

namespace pcb_ops {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::Pad;
using kicad_model::PcbTrack;
using kicad_model::PcbVia;
using kicad_model::GrPolygon;
using kicad_model::ItemType;
using geom::VECTOR2I;

namespace {

VECTOR2I world_pad(const Footprint & fp, const Pad & p) {
    double a = fp.angle.rad();
    double c = std::cos(a), s = std::sin(a);
    long long lx = p.at.x, ly = p.at.y;
    return { fp.at.x + static_cast<long long>(std::llround(lx * c - ly * s)),
             fp.at.y + static_cast<long long>(std::llround(lx * s + ly * c)) };
}

// Build a teardrop polygon: an elongated triangle from `pad_pt` toward
// the far end of the track, tapered along the track. Simple 4-point
// tear shape: [pad_pt+left, pad_pt+right, hinge_left, hinge_right].
std::vector<VECTOR2I> tear_polygon(VECTOR2I pad_pt, VECTOR2I far_pt,
                                   long long track_width, double length_mult,
                                   double width_mult) {
    double dx = far_pt.x - pad_pt.x, dy = far_pt.y - pad_pt.y;
    double L  = std::hypot(dx, dy);
    if (L < 1e-6) return {};
    double ux = dx / L, uy = dy / L;
    double px = -uy, py = ux;             // perpendicular
    double base_half = track_width * 0.5 * width_mult;
    double tip_half  = track_width * 0.5;
    double reach     = std::min(L * 0.6, static_cast<double>(track_width) * length_mult);
    VECTOR2I hinge{ pad_pt.x + static_cast<long long>(std::llround(ux * reach)),
                    pad_pt.y + static_cast<long long>(std::llround(uy * reach)) };
    return {
        { pad_pt.x + static_cast<long long>(std::llround(px * base_half)),
          pad_pt.y + static_cast<long long>(std::llround(py * base_half)) },
        { pad_pt.x - static_cast<long long>(std::llround(px * base_half)),
          pad_pt.y - static_cast<long long>(std::llround(py * base_half)) },
        { hinge.x - static_cast<long long>(std::llround(px * tip_half)),
          hinge.y - static_cast<long long>(std::llround(py * tip_half)) },
        { hinge.x + static_cast<long long>(std::llround(px * tip_half)),
          hinge.y + static_cast<long long>(std::llround(py * tip_half)) }
    };
}

} // namespace

std::size_t generate_teardrops(Board & board, const TeardropOptions & opts) {
    std::vector<const Footprint*> fps;
    std::vector<const PcbVia*>    vias;
    std::vector<const PcbTrack*>  tracks;
    for (const auto & it : board.items) {
        switch (it->type) {
            case ItemType::PcbFootprint: fps   .push_back(static_cast<const Footprint*>(it.get())); break;
            case ItemType::PcbVia:       vias  .push_back(static_cast<const PcbVia*>(it.get()));    break;
            case ItemType::PcbTrack:     tracks.push_back(static_cast<const PcbTrack*>(it.get()));  break;
            default: break;
        }
    }

    std::size_t count = 0;
    auto push_tear = [&](VECTOR2I pad_pt, VECTOR2I far_pt, long long width, const std::string & layer) {
        auto pts = tear_polygon(pad_pt, far_pt, width, opts.length_ratio_of_width, opts.width_ratio_of_track);
        if (pts.empty()) return;
        auto g = std::make_shared<GrPolygon>();
        g->layer     = layer;
        g->fill_type = "solid";
        for (const auto & p : pts) g->outline.append(p);
        g->outline.set_closed(true);
        g->uuid = kicad_model::make_uuid();
        board.items.push_back(g);
        ++count;
    };

    for (const auto * t : tracks) {
        // Match endpoint against every pad and via.
        if (opts.include_pads) {
            for (const auto * fp : fps) {
                for (const auto & p : fp->pads) {
                    if (p.net != t->net || t->net == 0) continue;
                    VECTOR2I center = world_pad(*fp, p);
                    if (std::abs(center.x - t->start.x) < 100000 && std::abs(center.y - t->start.y) < 100000)
                        push_tear(t->start, t->end, t->width_nm, t->layer);
                    if (std::abs(center.x - t->end.x)   < 100000 && std::abs(center.y - t->end.y)   < 100000)
                        push_tear(t->end,   t->start, t->width_nm, t->layer);
                }
            }
        }
        if (opts.include_vias) {
            for (const auto * v : vias) {
                if (v->net != t->net || t->net == 0) continue;
                if (std::abs(v->at.x - t->start.x) < 100000 && std::abs(v->at.y - t->start.y) < 100000)
                    push_tear(t->start, t->end, t->width_nm, t->layer);
                if (std::abs(v->at.x - t->end.x) < 100000 && std::abs(v->at.y - t->end.y) < 100000)
                    push_tear(t->end,   t->start, t->width_nm, t->layer);
            }
        }
    }
    return count;
}

namespace {
VECTOR2I item_origin(const kicad_model::Item & it) {
    switch (it.type) {
        case ItemType::PcbFootprint: return static_cast<const Footprint&>(it).at;
        case ItemType::PcbVia:       return static_cast<const PcbVia&>(it).at;
        default:                     return {0, 0};
    }
}
void set_item_origin(kicad_model::Item & it, VECTOR2I p) {
    switch (it.type) {
        case ItemType::PcbFootprint: static_cast<Footprint&>(it).at = p; return;
        case ItemType::PcbVia:       static_cast<PcbVia&>(it).at    = p; return;
        default: return;
    }
}
} // namespace

void align(Board & board, const std::vector<std::string> & uuids, AlignAxis axis) {
    std::unordered_set<std::string> want(uuids.begin(), uuids.end());
    std::vector<kicad_model::Item*> hits;
    for (auto & it : board.items) if (want.count(it->uuid)) hits.push_back(it.get());
    if (hits.empty()) return;

    long long ref = 0;
    switch (axis) {
        case AlignAxis::LeftX:   ref = item_origin(*hits[0]).x; for (auto * h : hits) ref = std::min(ref, item_origin(*h).x); break;
        case AlignAxis::RightX:  ref = item_origin(*hits[0]).x; for (auto * h : hits) ref = std::max(ref, item_origin(*h).x); break;
        case AlignAxis::TopY:    ref = item_origin(*hits[0]).y; for (auto * h : hits) ref = std::min(ref, item_origin(*h).y); break;
        case AlignAxis::BottomY: ref = item_origin(*hits[0]).y; for (auto * h : hits) ref = std::max(ref, item_origin(*h).y); break;
        case AlignAxis::CenterX:
        case AlignAxis::CenterY: {
            long long sum = 0;
            for (auto * h : hits) sum += (axis == AlignAxis::CenterX ? item_origin(*h).x : item_origin(*h).y);
            ref = sum / static_cast<long long>(hits.size());
            break;
        }
    }
    for (auto * h : hits) {
        auto p = item_origin(*h);
        if (axis == AlignAxis::LeftX || axis == AlignAxis::RightX || axis == AlignAxis::CenterX) p.x = ref;
        if (axis == AlignAxis::TopY  || axis == AlignAxis::BottomY || axis == AlignAxis::CenterY) p.y = ref;
        set_item_origin(*h, p);
    }
}

void distribute(Board & board, const std::vector<std::string> & uuids, bool horizontal) {
    std::unordered_set<std::string> want(uuids.begin(), uuids.end());
    std::vector<kicad_model::Item*> hits;
    for (auto & it : board.items) if (want.count(it->uuid)) hits.push_back(it.get());
    if (hits.size() < 3) return;

    std::sort(hits.begin(), hits.end(), [&](kicad_model::Item * a, kicad_model::Item * b) {
        return horizontal
            ? item_origin(*a).x < item_origin(*b).x
            : item_origin(*a).y < item_origin(*b).y;
    });
    long long lo = horizontal ? item_origin(*hits.front()).x : item_origin(*hits.front()).y;
    long long hi = horizontal ? item_origin(*hits.back ()).x : item_origin(*hits.back ()).y;
    long long span = hi - lo;
    long long step = span / (hits.size() - 1);
    for (std::size_t i = 1; i + 1 < hits.size(); ++i) {
        auto p = item_origin(*hits[i]);
        if (horizontal) p.x = lo + step * static_cast<long long>(i);
        else            p.y = lo + step * static_cast<long long>(i);
        set_item_origin(*hits[i], p);
    }
}

std::vector<std::string> pcb_uuids_for_ref(const Board & board, std::string_view reference) {
    std::vector<std::string> out;
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        for (const auto & f : fp->fields) {
            if (f.name == "Reference" && f.value == reference) {
                out.push_back(fp->uuid);
                break;
            }
        }
    }
    return out;
}
std::vector<std::string> sch_uuids_for_ref(const kicad_model::Schematic & sch, std::string_view reference) {
    std::vector<std::string> out;
    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        const auto * s = static_cast<const kicad_model::SchSymbol*>(it.get());
        if (s->reference() == reference) out.push_back(s->uuid);
    }
    return out;
}

} // namespace pcb_ops
