// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1515_diet_and_nutrition_formulator/diet_and_nutrition_formulator.hpp"

namespace {

testing::TestOutcome run_diet_and_nutrition_formulator() {
    diet_and_nutrition_formulator::init();
    auto s = diet_and_nutrition_formulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    diet_and_nutrition_formulator::shutdown();
    return testing::ok();
}

const int _reg_diet_and_nutrition_formulator = testing::register_test(
    "diet_and_nutrition_formulator",
    "1515_diet_and_nutrition_formulator: stub status check",
    &run_diet_and_nutrition_formulator);

}
