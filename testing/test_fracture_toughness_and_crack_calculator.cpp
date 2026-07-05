// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1003_fracture_toughness_and_crack_calculator/fracture_toughness_and_crack_calculator.hpp"

namespace {

testing::TestOutcome run_fracture_toughness_and_crack_calculator() {
    fracture_toughness_and_crack_calculator::init();
    auto s = fracture_toughness_and_crack_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fracture_toughness_and_crack_calculator::shutdown();
    return testing::ok();
}

const int _reg_fracture_toughness_and_crack_calculator = testing::register_test(
    "fracture_toughness_and_crack_calculator",
    "1003_fracture_toughness_and_crack_calculator: stub status check",
    &run_fracture_toughness_and_crack_calculator);

}
