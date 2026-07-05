// SPDX-License-Identifier: GPL-3.0-or-later
#include "pns_shove.hpp"

#include "../365_fast_drc/fast_drc.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

namespace pns_shove {

using kicad_model::Board;
using kicad_model::PcbTrack;
using kicad_model::PcbVia;
using kicad_model::UUID;
using kicad_model::ItemType;
using geom::VECTOR2I;

namespace {

// Direction unit vector from a to b. Falls back to (1, 0) for zero-length.
std::pair<double, double> unit_dir(VECTOR2I a, VECTOR2I b) {
    double dx = static_cast<double>(b.x - a.x), dy = static_cast<double>(b.y - a.y);
    double L  = std::hypot(dx, dy);
    if (L < 1.0) return { 1.0, 0.0 };
    return { dx / L, dy / L };
}

// Perpendicular to unit (dx, dy) pointing toward `from` from the segment.
VECTOR2I perpendicular_from(VECTOR2I seg_a, VECTOR2I seg_b, VECTOR2I from) {
    auto d = unit_dir(seg_a, seg_b);
    // Perp is (-dy, dx). Choose sign that points toward `from`.
    double px = -d.second, py = d.first;
    double fx = static_cast<double>(from.x - seg_a.x);
    double fy = static_cast<double>(from.y - seg_a.y);
    if (px * fx + py * fy < 0) { px = -px; py = -py; }
    return { static_cast<long long>(std::llround(px * 1e6)),
             static_cast<long long>(std::llround(py * 1e6)) };
}

// Translate a track by dx/dy in place. Returns previous endpoints for undo.
std::pair<VECTOR2I, VECTOR2I> shove_track(PcbTrack & t, long long dx, long long dy) {
    auto prev = std::make_pair(t.start, t.end);
    t.start.x += dx; t.start.y += dy;
    t.end.x   += dx; t.end.y   += dy;
    return prev;
}

double sqmag(VECTOR2I v) {
    double x = static_cast<double>(v.x), y = static_cast<double>(v.y);
    return x * x + y * y;
}

bool track_has_collisions(const Board & board, const PcbTrack & t, const Config & cfg) {
    fast_drc::ProposedTrack p;
    p.start   = t.start;
    p.end     = t.end;
    p.width_nm= t.width_nm;
    p.layer   = t.layer;
    p.net     = t.net;
    fast_drc::Config fc; fc.base_clearance_mm = cfg.clearance_mm;
    return !fast_drc::check_track(board, p, fc).empty();
}

// Compute how far (in nm) obstacle track `t` needs to move perpendicular
// to itself to restore clearance from `prop`.
long long required_shove_distance(const PcbTrack & prop, const PcbTrack & t, double clearance_mm) {
    geom::SEG a{ prop.start, prop.end };
    // Distance from prop's midpoint to obstacle segment.
    VECTOR2I mid{ (prop.start.x + prop.end.x) / 2, (prop.start.y + prop.end.y) / 2 };
    geom::SEG b{ t.start, t.end };
    double d = b.distance(mid);
    // Target clearance = half widths + rule.
    double need = clearance_mm * 1e6 + prop.width_nm / 2.0 + t.width_nm / 2.0;
    double delta = need - d;
    return std::max<long long>(0, static_cast<long long>(std::ceil(delta)) + 10000); // +10 um margin
}

bool recursive_check(Board & board, const PcbTrack & new_t, const Config & cfg,
                     std::vector<ShoveOp> & shoves, int depth) {
    if (depth > cfg.max_depth) return false;
    fast_drc::ProposedTrack p;
    p.start=new_t.start; p.end=new_t.end;
    p.width_nm=new_t.width_nm; p.layer=new_t.layer; p.net=new_t.net;
    fast_drc::Config fc; fc.base_clearance_mm = cfg.clearance_mm;
    auto hits = fast_drc::check_track(board, p, fc);
    if (hits.empty()) return true;

    // Try to shove each collider that is a movable PcbTrack.
    for (const auto & h : hits) {
        PcbTrack * obst = nullptr;
        for (auto & it : board.items) {
            if (it->uuid == h.other_uuid && it->type == ItemType::PcbTrack) {
                obst = static_cast<PcbTrack*>(it.get()); break;
            }
        }
        if (!obst) return false;   // pad / via / non-track: can't shove

        // Compute displacement.
        long long dist = required_shove_distance(new_t, *obst, cfg.clearance_mm);
        if (dist <= 0) continue;
        VECTOR2I dir = perpendicular_from(new_t.start, new_t.end, obst->start);
        // Normalize.
        double L = std::sqrt(sqmag(dir));
        if (L < 1.0) return false;
        long long dx = static_cast<long long>(std::llround(dir.x / L * dist));
        long long dy = static_cast<long long>(std::llround(dir.y / L * dist));

        double range_nm = cfg.shove_range_mm * 1e6;
        if (std::abs(dx) > range_nm || std::abs(dy) > range_nm) return false;

        auto prev = shove_track(*obst, dx, dy);
        shoves.push_back({ obst->uuid, dx, dy });

        if (!recursive_check(board, new_t, cfg, shoves, depth + 1)) {
            // Undo this shove.
            obst->start = prev.first; obst->end = prev.second;
            shoves.pop_back();
            return false;
        }
    }
    return true;
}

} // namespace

Result add_track(Board & board, const PcbTrack & prop, const Config & cfg) {
    Result r;
    auto t = std::make_shared<PcbTrack>(prop);
    if (t->uuid.empty()) t->uuid = kicad_model::make_uuid();

    // Fast path: no collision.
    if (!track_has_collisions(board, *t, cfg)) {
        r.new_track_index = board.items.size();
        board.items.push_back(t);
        r.ok = true;
        return r;
    }
    // Shove path.
    std::vector<ShoveOp> shoves;
    // Temporarily add the track so recursive_check can see it? No,
    // recursive_check treats it via `new_t` param separately. So we
    // pass `*t` to check without adding it yet.
    if (!recursive_check(board, *t, cfg, shoves, 0)) {
        // Undo all applied shoves.
        for (auto it = shoves.rbegin(); it != shoves.rend(); ++it) {
            for (auto & item : board.items) {
                if (item->uuid == it->uuid && item->type == ItemType::PcbTrack) {
                    auto * pt = static_cast<PcbTrack*>(item.get());
                    pt->start.x -= it->dx_nm; pt->start.y -= it->dy_nm;
                    pt->end.x   -= it->dx_nm; pt->end.y   -= it->dy_nm;
                    break;
                }
            }
        }
        r.reason = "unshoveable obstacle (max_depth reached or non-track blocker)";
        return r;
    }
    r.new_track_index = board.items.size();
    board.items.push_back(t);
    r.applied_shoves = std::move(shoves);
    r.ok = true;
    return r;
}

Result drag_track(Board & board, const UUID & uuid, long long dx, long long dy, const Config & cfg) {
    Result r;
    PcbTrack * t = nullptr;
    for (auto & it : board.items) {
        if (it->uuid == uuid && it->type == ItemType::PcbTrack) {
            t = static_cast<PcbTrack*>(it.get()); break;
        }
    }
    if (!t) { r.reason = "track not found"; return r; }
    auto prev = shove_track(*t, dx, dy);
    if (!track_has_collisions(board, *t, cfg)) {
        r.ok = true;
        return r;
    }
    std::vector<ShoveOp> shoves;
    if (!recursive_check(board, *t, cfg, shoves, 0)) {
        // Revert.
        t->start = prev.first; t->end = prev.second;
        for (auto it = shoves.rbegin(); it != shoves.rend(); ++it) {
            for (auto & item : board.items) {
                if (item->uuid == it->uuid && item->type == ItemType::PcbTrack) {
                    auto * pt = static_cast<PcbTrack*>(item.get());
                    pt->start.x -= it->dx_nm; pt->start.y -= it->dy_nm;
                    pt->end.x   -= it->dx_nm; pt->end.y   -= it->dy_nm;
                    break;
                }
            }
        }
        r.reason = "drag blocked by unshoveable obstacle";
        return r;
    }
    r.applied_shoves = std::move(shoves);
    r.ok = true;
    return r;
}

} // namespace pns_shove
