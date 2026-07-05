// SPDX-License-Identifier: GPL-3.0-or-later
#include "length_tuning.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

namespace length_tuning {

using kicad_model::Board;
using kicad_model::PcbTrack;
using kicad_model::UUID;
using kicad_model::ItemType;
using geom::VECTOR2I;
using geom::mm_to_nm;
using geom::nm_to_mm;

namespace {

double path_length_mm(const std::vector<VECTOR2I> & path) {
    double L = 0.0;
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        double dx = static_cast<double>(path[i+1].x - path[i].x);
        double dy = static_cast<double>(path[i+1].y - path[i].y);
        L += std::hypot(dx, dy);
    }
    return nm_to_mm(static_cast<long long>(L));
}

// Segment along direction (ux, uy) from origin; ptOfs is orthogonal offset.
VECTOR2I project(VECTOR2I origin, double ux, double uy, double along_mm, double ortho_mm) {
    return {
        origin.x + mm_to_nm(along_mm * ux + ortho_mm * -uy),
        origin.y + mm_to_nm(along_mm * uy + ortho_mm *  ux)
    };
}

} // namespace

MeanderResult compute(VECTOR2I a, VECTOR2I b, const MeanderOptions & opts) {
    MeanderResult r;
    double dx = static_cast<double>(b.x - a.x), dy = static_cast<double>(b.y - a.y);
    double base_len_mm = nm_to_mm(static_cast<long long>(std::hypot(dx, dy)));
    if (base_len_mm < 1e-6) {
        r.reason = "zero-length base"; return r;
    }
    if (opts.target_length_mm <= base_len_mm + 1e-6) {
        r.path = { a, b };
        r.achieved_length_mm = base_len_mm;
        r.ok = true;
        return r;
    }
    double extra_needed_mm = opts.target_length_mm - base_len_mm;
    // Meander cell adds (2 * amplitude) of extra length per period.
    // We fit as many periods as needed.
    double per_period_extra = 2.0 * opts.amplitude_mm;
    int    periods = static_cast<int>(std::ceil(extra_needed_mm / per_period_extra));
    if (periods < 1) periods = 1;

    // Distribute the periods along the base line, centered on the middle.
    double occupied_mm = periods * opts.period_mm;
    if (occupied_mm > base_len_mm) {
        r.reason = "target length requires more meanders than base fits; try longer period";
        return r;
    }
    double lead_in  = (base_len_mm - occupied_mm) / 2.0;
    double lead_out = lead_in;

    // Unit vector along base.
    double L = base_len_mm;
    double ux = dx / (L * 1e6), uy = dy / (L * 1e6);
    // Envelope check (only checks endpoint peaks).
    double amp = opts.amplitude_mm;
    if (opts.envelope_max_y_mm > opts.envelope_min_y_mm) {
        double ay = nm_to_mm(a.y);
        if (ay - amp < opts.envelope_min_y_mm || ay + amp > opts.envelope_max_y_mm) {
            r.reason = "meander amplitude would leave envelope";
            return r;
        }
    }

    // Build path: lead-in straight, then N periods of triangle wave, then lead-out.
    r.path.push_back(a);
    double t = lead_in;
    r.path.push_back(project(a, ux, uy, t, 0));
    for (int i = 0; i < periods; ++i) {
        double sign = (i % 2 == 0) ? 1.0 : -1.0;
        r.path.push_back(project(a, ux, uy, t + opts.period_mm * 0.25, sign * amp));
        r.path.push_back(project(a, ux, uy, t + opts.period_mm * 0.75, -sign * amp));
        t += opts.period_mm;
    }
    r.path.push_back(project(a, ux, uy, t, 0));
    r.path.push_back(b);
    r.achieved_length_mm = path_length_mm(r.path);
    r.ok = true;
    return r;
}

ApplyResult apply(Board & board, const UUID & uuid, const MeanderOptions & opts) {
    ApplyResult out;
    PcbTrack * t = nullptr;
    std::size_t idx = 0;
    for (std::size_t i = 0; i < board.items.size(); ++i) {
        if (board.items[i]->uuid == uuid && board.items[i]->type == ItemType::PcbTrack) {
            t = static_cast<PcbTrack*>(board.items[i].get());
            idx = i;
            break;
        }
    }
    if (!t) { out.reason = "track not found"; return out; }

    auto meander = compute(t->start, t->end, opts);
    if (!meander.ok) { out.reason = meander.reason; return out; }

    // Copy parameters.
    long long width  = t->width_nm;
    std::string layer= t->layer;
    int net          = t->net;

    // Remove original.
    board.items.erase(board.items.begin() + idx);

    // Insert segments.
    for (std::size_t i = 0; i + 1 < meander.path.size(); ++i) {
        auto seg = std::make_shared<PcbTrack>();
        seg->start = meander.path[i];
        seg->end   = meander.path[i + 1];
        seg->width_nm = width;
        seg->layer    = layer;
        seg->net      = net;
        seg->uuid     = kicad_model::make_uuid();
        board.items.push_back(seg);
        ++out.segments_added;
    }
    out.achieved_length_mm = meander.achieved_length_mm;
    out.ok = true;
    return out;
}

} // namespace length_tuning
