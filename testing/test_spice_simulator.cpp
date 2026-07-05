// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/849_spice_simulator. Simulates an RC lowpass.

#include "test_runner.hpp"
#include "../modules/849_spice_simulator/spice_simulator.hpp"

#include <algorithm>
#include <string>

namespace {

testing::TestOutcome run() {
    spice_simulator::init();
    if (!spice_simulator::available()) {
        return testing::skip("libngspice not present at runtime");
    }
    auto r = spice_simulator::run(spice_simulator::sample_rc_netlist(),
                                  "tran 1u 200u");
    if (!r.ok) return testing::fail("run failed: " + r.error + "\nlog=" + r.log);

    bool has_time = false, has_out = false;
    for (const auto & s : r.signals) {
        std::string lower = s.name;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        if (lower == "time") has_time = true;
        if (lower.find("out") != std::string::npos) has_out = true;
    }
    if (!has_time) return testing::fail("no time vector in output");
    if (!has_out)  return testing::fail("no V(out) vector in output");

    spice_simulator::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "spice_simulator",
    "Load libngspice; simulate an RC lowpass transient; assert time + V(out) vectors present.",
    &run);

} // namespace
