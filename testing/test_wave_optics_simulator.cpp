// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/906_wave_optics_simulator/wave_optics_simulator.hpp"

namespace {

testing::TestOutcome run_wave_optics_simulator() {
    wave_optics_simulator::init();
    auto s = wave_optics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wave_optics_simulator::shutdown();
    return testing::ok();
}

const int _reg_wave_optics_simulator = testing::register_test(
    "wave_optics_simulator",
    "906_wave_optics_simulator: stub status check",
    &run_wave_optics_simulator);

}
