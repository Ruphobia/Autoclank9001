// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2073_x_1rm_and_load_calculator/x_1rm_and_load_calculator.hpp"

namespace {

testing::TestOutcome run_x_1rm_and_load_calculator() {
    x_1rm_and_load_calculator::init();
    auto s = x_1rm_and_load_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    x_1rm_and_load_calculator::shutdown();
    return testing::ok();
}

const int _reg_x_1rm_and_load_calculator = testing::register_test(
    "x_1rm_and_load_calculator",
    "2073_x_1rm_and_load_calculator: stub status check",
    &run_x_1rm_and_load_calculator);

}
