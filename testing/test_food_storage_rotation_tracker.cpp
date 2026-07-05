// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2118_food_storage_rotation_tracker/food_storage_rotation_tracker.hpp"

namespace {

testing::TestOutcome run_food_storage_rotation_tracker() {
    food_storage_rotation_tracker::init();
    auto s = food_storage_rotation_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    food_storage_rotation_tracker::shutdown();
    return testing::ok();
}

const int _reg_food_storage_rotation_tracker = testing::register_test(
    "food_storage_rotation_tracker",
    "2118_food_storage_rotation_tracker: stub status check",
    &run_food_storage_rotation_tracker);

}
