// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../368_ibis/ibis.hpp"

#include <string>
#include <string_view>
#include <vector>

// IBIS-driven timing checks.
//
// Uses the parsed IBIS File model (from 368_ibis) to compute:
//   * driver strength: derived from the Pulldown/Pullup IV curves
//     (Vcc / min |di/dv|).
//   * pin capacitance: C_pkg + C_comp.
//   * approximate RC rise/fall from Rise Waveform ramp gradient.
//
// The MVP is a metric emitter; a full timing-margin analyzer that
// checks setup/hold against clock-tree constraints is a follow-up.
namespace ibis_timing {

struct Metrics {
    std::string model_name;
    double vinh          = 0.0;
    double vinl          = 0.0;
    double c_comp_pF     = 0.0;
    double drive_impedance_ohm = 0.0;   // 1 / max_gradient
    double rise_time_ns  = 0.0;         // 10%-90% from Rising Waveform
    double fall_time_ns  = 0.0;
    std::vector<std::string> warnings;
};

Metrics evaluate(const ibis::File & f, std::string_view model_name);

}
