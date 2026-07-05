// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2023_manual_j_load_calculator/manual_j_load_calculator.hpp"

namespace {

testing::TestOutcome run_manual_j_load_calculator() {
    manual_j_load_calculator::init();
    auto s = manual_j_load_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manual_j_load_calculator::shutdown();
    return testing::ok();
}

const int _reg_manual_j_load_calculator = testing::register_test(
    "manual_j_load_calculator",
    "2023_manual_j_load_calculator: stub status check",
    &run_manual_j_load_calculator);

}
