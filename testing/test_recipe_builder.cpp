// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2075_recipe_builder/recipe_builder.hpp"

namespace {

testing::TestOutcome run_recipe_builder() {
    recipe_builder::init();
    auto s = recipe_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    recipe_builder::shutdown();
    return testing::ok();
}

const int _reg_recipe_builder = testing::register_test(
    "recipe_builder",
    "2075_recipe_builder: stub status check",
    &run_recipe_builder);

}
