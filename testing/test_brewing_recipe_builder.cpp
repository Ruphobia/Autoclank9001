// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1788_brewing_recipe_builder/brewing_recipe_builder.hpp"

namespace {

testing::TestOutcome run_brewing_recipe_builder() {
    brewing_recipe_builder::init();
    auto s = brewing_recipe_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    brewing_recipe_builder::shutdown();
    return testing::ok();
}

const int _reg_brewing_recipe_builder = testing::register_test(
    "brewing_recipe_builder",
    "1788_brewing_recipe_builder: stub status check",
    &run_brewing_recipe_builder);

}
