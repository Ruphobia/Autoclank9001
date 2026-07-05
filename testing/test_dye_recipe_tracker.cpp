#include "test_runner.hpp"
#include "../modules/1776_dye_recipe_tracker/dye_recipe_tracker.hpp"

namespace {

testing::TestOutcome run_dye_recipe_tracker() {
    dye_recipe_tracker::init();
    auto s = dye_recipe_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dye_recipe_tracker::shutdown();
    return testing::ok();
}

const int _reg_dye_recipe_tracker = testing::register_test(
    "dye_recipe_tracker",
    "1776_dye_recipe_tracker: stub status check",
    &run_dye_recipe_tracker);

}
