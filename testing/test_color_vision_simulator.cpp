// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1865_color_vision_simulator/color_vision_simulator.hpp"

namespace {

testing::TestOutcome run_color_vision_simulator() {
    color_vision_simulator::init();
    auto s = color_vision_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_vision_simulator::shutdown();
    return testing::ok();
}

const int _reg_color_vision_simulator = testing::register_test(
    "color_vision_simulator",
    "1865_color_vision_simulator: stub status check",
    &run_color_vision_simulator);

}
