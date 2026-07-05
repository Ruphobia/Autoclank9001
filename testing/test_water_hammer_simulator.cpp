// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1024_water_hammer_simulator/water_hammer_simulator.hpp"

namespace {

testing::TestOutcome run_water_hammer_simulator() {
    water_hammer_simulator::init();
    auto s = water_hammer_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    water_hammer_simulator::shutdown();
    return testing::ok();
}

const int _reg_water_hammer_simulator = testing::register_test(
    "water_hammer_simulator",
    "1024_water_hammer_simulator: stub status check",
    &run_water_hammer_simulator);

}
