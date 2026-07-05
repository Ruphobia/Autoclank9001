// SPDX-License-Identifier: GPL-3.0-or-later
#include "ibis_timing.hpp"

#include <algorithm>
#include <cmath>

namespace ibis_timing {

namespace {

double compute_transition_ns(const ibis::VTTable & vt) {
    if (vt.points.size() < 3) return 0.0;
    double v_min = vt.points.front().second, v_max = v_min;
    for (const auto & p : vt.points) { v_min = std::min(v_min, p.second); v_max = std::max(v_max, p.second); }
    double lo = v_min + 0.1 * (v_max - v_min);
    double hi = v_min + 0.9 * (v_max - v_min);
    double t_lo = vt.points.front().first, t_hi = vt.points.back().first;
    for (std::size_t i = 0; i + 1 < vt.points.size(); ++i) {
        if ((vt.points[i].second <= lo) != (vt.points[i + 1].second <= lo)) t_lo = vt.points[i].first;
        if ((vt.points[i].second <= hi) != (vt.points[i + 1].second <= hi)) t_hi = vt.points[i + 1].first;
    }
    return (t_hi - t_lo) * 1e9;
}

double compute_drive_impedance(const std::vector<ibis::IVTable> & iv) {
    // Look for Pullup table; slope dV/dI at the mid-range gives Ron.
    for (const auto & tbl : iv) {
        if (tbl.kind != "Pullup") continue;
        if (tbl.points.size() < 2) continue;
        double best_slope = 0.0;
        for (std::size_t i = 0; i + 1 < tbl.points.size(); ++i) {
            double dV = tbl.points[i + 1].first  - tbl.points[i].first;
            double dI = tbl.points[i + 1].second - tbl.points[i].second;
            if (std::abs(dI) > 1e-15) {
                double slope = std::abs(dV / dI);
                best_slope = std::max(best_slope, slope);
            }
        }
        return best_slope;
    }
    return 0.0;
}

} // namespace

Metrics evaluate(const ibis::File & f, std::string_view model_name) {
    Metrics m;
    const ibis::Model * mo = nullptr;
    for (const auto & mm : f.models) {
        if (mm.name == model_name) { mo = &mm; break; }
    }
    if (!mo) {
        // Fallback to first model.
        if (f.models.empty()) { m.warnings.push_back("no models present"); return m; }
        mo = &f.models.front();
    }
    m.model_name = mo->name;
    m.vinh       = mo->vinh;
    m.vinl       = mo->vinl;
    m.c_comp_pF  = mo->c_comp_pF;
    m.drive_impedance_ohm = compute_drive_impedance(mo->iv);
    for (const auto & vt : mo->vt) {
        double dt = compute_transition_ns(vt);
        if (vt.kind == "Rising Waveform")  m.rise_time_ns = dt;
        if (vt.kind == "Falling Waveform") m.fall_time_ns = dt;
    }
    return m;
}

} // namespace ibis_timing
