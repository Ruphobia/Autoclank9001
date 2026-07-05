// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1818_recipe_scaling_and_yield_tracker/recipe_scaling_and_yield_tracker.hpp"

namespace {

testing::TestOutcome run_recipe_scaling_and_yield_tracker() {
    recipe_scaling_and_yield_tracker::init();
    auto s = recipe_scaling_and_yield_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    recipe_scaling_and_yield_tracker::shutdown();
    return testing::ok();
}

const int _reg_recipe_scaling_and_yield_tracker = testing::register_test(
    "recipe_scaling_and_yield_tracker",
    "1818_recipe_scaling_and_yield_tracker: stub status check",
    &run_recipe_scaling_and_yield_tracker);

}
