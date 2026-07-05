// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1311_solution_and_buffer_calculator/solution_and_buffer_calculator.hpp"

namespace {

testing::TestOutcome run_solution_and_buffer_calculator() {
    solution_and_buffer_calculator::init();
    auto s = solution_and_buffer_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    solution_and_buffer_calculator::shutdown();
    return testing::ok();
}

const int _reg_solution_and_buffer_calculator = testing::register_test(
    "solution_and_buffer_calculator",
    "1311_solution_and_buffer_calculator: stub status check",
    &run_solution_and_buffer_calculator);

}
