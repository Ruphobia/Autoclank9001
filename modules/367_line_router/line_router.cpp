// SPDX-License-Identifier: GPL-3.0-or-later
#include "line_router.hpp"

#include "../365_fast_drc/fast_drc.hpp"

#include <cmath>
#include <memory>
#include <sstream>
#include <unordered_map>

namespace line_router {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::Pad;
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

bool segment_clear(const Board & board, VECTOR2I a, VECTOR2I b, int net,
                   long long width_nm, const std::string & layer,
                   double clearance_mm) {
    fast_drc::ProposedTrack pt;
    pt.start = a; pt.end = b;
    pt.width_nm = width_nm;
    pt.layer = layer;
    pt.net = net;
    fast_drc::Config cfg;
    cfg.base_clearance_mm = clearance_mm;
    auto hits = fast_drc::check_track(board, pt, cfg);
    return hits.empty();
}

bool add_segment(Board & board, VECTOR2I a, VECTOR2I b, int net,
                 long long width_nm, const std::string & layer) {
    if (a == b) return true;
    auto t = std::make_shared<kicad_model::PcbTrack>();
    t->start = a; t->end = b;
    t->width_nm = width_nm;
    t->layer = layer;
    t->net = net;
    t->uuid = kicad_model::make_uuid();
    board.items.push_back(t);
    return true;
}

} // namespace

SingleResult route_pair(Board & board, int net_id, VECTOR2I a, VECTOR2I b, const Config & cfg) {
    SingleResult out;
    if (a == b) { out.routed = true; out.reason = "same_point"; return out; }

    // Candidate 1: horizontal, then vertical.
    VECTOR2I mid1{ b.x, a.y };
    bool ok1 = segment_clear(board, a, mid1, net_id, cfg.track_width_nm, cfg.layer, cfg.clearance_mm)
            && segment_clear(board, mid1, b, net_id, cfg.track_width_nm, cfg.layer, cfg.clearance_mm);
    if (ok1) {
        add_segment(board, a, mid1, net_id, cfg.track_width_nm, cfg.layer);
        add_segment(board, mid1, b, net_id, cfg.track_width_nm, cfg.layer);
        out.routed = true; out.reason = "clear";
        return out;
    }
    // Candidate 2: vertical, then horizontal.
    VECTOR2I mid2{ a.x, b.y };
    bool ok2 = segment_clear(board, a, mid2, net_id, cfg.track_width_nm, cfg.layer, cfg.clearance_mm)
            && segment_clear(board, mid2, b, net_id, cfg.track_width_nm, cfg.layer, cfg.clearance_mm);
    if (ok2) {
        add_segment(board, a, mid2, net_id, cfg.track_width_nm, cfg.layer);
        add_segment(board, mid2, b, net_id, cfg.track_width_nm, cfg.layer);
        out.routed = true; out.reason = "clear";
        return out;
    }
    out.reason = "obstructed";
    return out;
}

BatchResult route_all_unrouted(Board & board, const Config & cfg) {
    BatchResult out;
    // Nets already having any track are skipped.
    std::unordered_map<int, bool> has_track;
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbTrack) continue;
        has_track[static_cast<kicad_model::PcbTrack*>(it.get())->net] = true;
    }
    // Collect pad endpoints per net.
    std::unordered_map<int, std::vector<VECTOR2I>> pads;
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        for (const auto & p : fp->pads) {
            if (p.net <= 0) continue;
            pads[p.net].push_back(world_pad(*fp, p));
        }
    }
    std::ostringstream log;
    for (auto & kv : pads) {
        if (has_track[kv.first]) continue;
        if (kv.second.size() < 2) continue;
        // MST via nearest-neighbor from the first pad.
        std::vector<VECTOR2I> remaining(kv.second.begin() + 1, kv.second.end());
        VECTOR2I current = kv.second.front();
        while (!remaining.empty()) {
            std::size_t best = 0;
            double bestd = 1e30;
            for (std::size_t i = 0; i < remaining.size(); ++i) {
                double d = std::hypot(static_cast<double>(remaining[i].x - current.x),
                                       static_cast<double>(remaining[i].y - current.y));
                if (d < bestd) { bestd = d; best = i; }
            }
            auto r = route_pair(board, kv.first, current, remaining[best], cfg);
            if (r.routed) { ++out.routed_pairs; log << "net " << kv.first << ": ok\n"; }
            else          { ++out.obstructed_pairs; log << "net " << kv.first << ": obstructed\n"; }
            current = remaining[best];
            remaining.erase(remaining.begin() + best);
        }
    }
    out.log = log.str();
    return out;
}

} // namespace line_router
