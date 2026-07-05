// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1783_recipe_scaler/recipe_scaler.hpp"

namespace {

testing::TestOutcome run_recipe_scaler() {
    recipe_scaler::init();
    auto s = recipe_scaler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    recipe_scaler::shutdown();
    return testing::ok();
}

const int _reg_recipe_scaler = testing::register_test(
    "recipe_scaler",
    "1783_recipe_scaler: stub status check",
    &run_recipe_scaler);

}
