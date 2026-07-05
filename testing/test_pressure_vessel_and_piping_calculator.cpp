// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/961_pressure_vessel_and_piping_calculator/pressure_vessel_and_piping_calculator.hpp"

namespace {

testing::TestOutcome run_pressure_vessel_and_piping_calculator() {
    pressure_vessel_and_piping_calculator::init();
    auto s = pressure_vessel_and_piping_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pressure_vessel_and_piping_calculator::shutdown();
    return testing::ok();
}

const int _reg_pressure_vessel_and_piping_calculator = testing::register_test(
    "pressure_vessel_and_piping_calculator",
    "961_pressure_vessel_and_piping_calculator: stub status check",
    &run_pressure_vessel_and_piping_calculator);

}
