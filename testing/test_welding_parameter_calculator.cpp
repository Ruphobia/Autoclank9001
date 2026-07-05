// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/978_welding_parameter_calculator/welding_parameter_calculator.hpp"

namespace {

testing::TestOutcome run_welding_parameter_calculator() {
    welding_parameter_calculator::init();
    auto s = welding_parameter_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    welding_parameter_calculator::shutdown();
    return testing::ok();
}

const int _reg_welding_parameter_calculator = testing::register_test(
    "welding_parameter_calculator",
    "978_welding_parameter_calculator: stub status check",
    &run_welding_parameter_calculator);

}
