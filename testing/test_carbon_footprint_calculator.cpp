// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1568_carbon_footprint_calculator/carbon_footprint_calculator.hpp"

namespace {

testing::TestOutcome run_carbon_footprint_calculator() {
    carbon_footprint_calculator::init();
    auto s = carbon_footprint_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    carbon_footprint_calculator::shutdown();
    return testing::ok();
}

const int _reg_carbon_footprint_calculator = testing::register_test(
    "carbon_footprint_calculator",
    "1568_carbon_footprint_calculator: stub status check",
    &run_carbon_footprint_calculator);

}
