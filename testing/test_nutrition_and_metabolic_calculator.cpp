// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1417_nutrition_and_metabolic_calculator/nutrition_and_metabolic_calculator.hpp"

namespace {

testing::TestOutcome run_nutrition_and_metabolic_calculator() {
    nutrition_and_metabolic_calculator::init();
    auto s = nutrition_and_metabolic_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nutrition_and_metabolic_calculator::shutdown();
    return testing::ok();
}

const int _reg_nutrition_and_metabolic_calculator = testing::register_test(
    "nutrition_and_metabolic_calculator",
    "1417_nutrition_and_metabolic_calculator: stub status check",
    &run_nutrition_and_metabolic_calculator);

}
