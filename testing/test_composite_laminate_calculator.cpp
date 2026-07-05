// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/997_composite_laminate_calculator/composite_laminate_calculator.hpp"

namespace {

testing::TestOutcome run_composite_laminate_calculator() {
    composite_laminate_calculator::init();
    auto s = composite_laminate_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    composite_laminate_calculator::shutdown();
    return testing::ok();
}

const int _reg_composite_laminate_calculator = testing::register_test(
    "composite_laminate_calculator",
    "997_composite_laminate_calculator: stub status check",
    &run_composite_laminate_calculator);

}
