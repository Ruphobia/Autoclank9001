// SPDX-License-Identifier: GPL-3.0-or-later
#include "diff_pair.hpp"

#include <cmath>
#include <memory>

namespace diff_pair {

using kicad_model::Board;
using kicad_model::PcbTrack;
using geom::VECTOR2I;

namespace {

void add_track(Board & board, VECTOR2I a, VECTOR2I b, long long w, const std::string & layer, int net) {
    if (a == b) return;
    auto t = std::make_shared<PcbTrack>();
    t->start = a; t->end = b; t->width_nm = w; t->layer = layer; t->net = net;
    t->uuid = kicad_model::make_uuid();
    board.items.push_back(t);
}

} // namespace

Result route(Board & board, VECTOR2I p_start, VECTOR2I p_end,
             VECTOR2I n_start, VECTOR2I n_end, const Config & cfg) {
    Result r;
    if (cfg.p_net == 0 || cfg.n_net == 0) {
        r.reason = "both nets required"; return r;
    }
    // Direction from p_start toward p_end.
    double dx = static_cast<double>(p_end.x - p_start.x);
    double dy = static_cast<double>(p_end.y - p_start.y);
    double L  = std::hypot(dx, dy);
    if (L < 1.0) { r.reason = "zero length p pair"; return r; }
    double ux = dx / L, uy = dy / L;
    // Perpendicular unit; positive side away from n.
    double px = -uy, py = ux;

    // Determine which side n is on so we route n on the same side.
    double nx = static_cast<double>(n_start.x - p_start.x);
    double ny = static_cast<double>(n_start.y - p_start.y);
    if (nx * px + ny * py < 0) { px = -px; py = -py; }

    long long offset = cfg.track_width_nm + cfg.gap_nm;

    // Emit the two straight tracks.
    add_track(board, p_start, p_end, cfg.track_width_nm, cfg.layer, cfg.p_net); ++r.p_segments;

    VECTOR2I nsA{ p_start.x + static_cast<long long>(std::llround(px * offset)),
                  p_start.y + static_cast<long long>(std::llround(py * offset)) };
    VECTOR2I nsB{ p_end.x   + static_cast<long long>(std::llround(px * offset)),
                  p_end.y   + static_cast<long long>(std::llround(py * offset)) };

    // Bridge from actual n endpoints to the offset track ends.
    add_track(board, n_start, nsA, cfg.track_width_nm, cfg.layer, cfg.n_net); ++r.n_segments;
    add_track(board, nsA,     nsB, cfg.track_width_nm, cfg.layer, cfg.n_net); ++r.n_segments;
    add_track(board, nsB,     n_end, cfg.track_width_nm, cfg.layer, cfg.n_net); ++r.n_segments;

    r.ok = true;
    return r;
}

} // namespace diff_pair
