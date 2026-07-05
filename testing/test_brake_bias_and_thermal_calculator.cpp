// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1214_brake_bias_and_thermal_calculator/brake_bias_and_thermal_calculator.hpp"

namespace {

testing::TestOutcome run_brake_bias_and_thermal_calculator() {
    brake_bias_and_thermal_calculator::init();
    auto s = brake_bias_and_thermal_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    brake_bias_and_thermal_calculator::shutdown();
    return testing::ok();
}

const int _reg_brake_bias_and_thermal_calculator = testing::register_test(
    "brake_bias_and_thermal_calculator",
    "1214_brake_bias_and_thermal_calculator: stub status check",
    &run_brake_bias_and_thermal_calculator);

}
