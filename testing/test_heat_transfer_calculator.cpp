// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/948_heat_transfer_calculator/heat_transfer_calculator.hpp"

namespace {

testing::TestOutcome run_heat_transfer_calculator() {
    heat_transfer_calculator::init();
    auto s = heat_transfer_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    heat_transfer_calculator::shutdown();
    return testing::ok();
}

const int _reg_heat_transfer_calculator = testing::register_test(
    "heat_transfer_calculator",
    "948_heat_transfer_calculator: stub status check",
    &run_heat_transfer_calculator);

}
