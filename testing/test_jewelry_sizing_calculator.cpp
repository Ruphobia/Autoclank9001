// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1902_jewelry_sizing_calculator/jewelry_sizing_calculator.hpp"

namespace {

testing::TestOutcome run_jewelry_sizing_calculator() {
    jewelry_sizing_calculator::init();
    auto s = jewelry_sizing_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    jewelry_sizing_calculator::shutdown();
    return testing::ok();
}

const int _reg_jewelry_sizing_calculator = testing::register_test(
    "jewelry_sizing_calculator",
    "1902_jewelry_sizing_calculator: stub status check",
    &run_jewelry_sizing_calculator);

}
