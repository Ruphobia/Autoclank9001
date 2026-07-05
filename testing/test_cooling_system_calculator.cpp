// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1223_cooling_system_calculator/cooling_system_calculator.hpp"

namespace {

testing::TestOutcome run_cooling_system_calculator() {
    cooling_system_calculator::init();
    auto s = cooling_system_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cooling_system_calculator::shutdown();
    return testing::ok();
}

const int _reg_cooling_system_calculator = testing::register_test(
    "cooling_system_calculator",
    "1223_cooling_system_calculator: stub status check",
    &run_cooling_system_calculator);

}
