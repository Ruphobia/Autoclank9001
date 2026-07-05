// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1229_tire_wheel_calculator/tire_wheel_calculator.hpp"

namespace {

testing::TestOutcome run_tire_wheel_calculator() {
    tire_wheel_calculator::init();
    auto s = tire_wheel_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tire_wheel_calculator::shutdown();
    return testing::ok();
}

const int _reg_tire_wheel_calculator = testing::register_test(
    "tire_wheel_calculator",
    "1229_tire_wheel_calculator: stub status check",
    &run_tire_wheel_calculator);

}
