// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/447_okr_and_goal_tracker/okr_and_goal_tracker.hpp"

namespace {

testing::TestOutcome run_okr_and_goal_tracker() {
    okr_and_goal_tracker::init();
    auto s = okr_and_goal_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    okr_and_goal_tracker::shutdown();
    return testing::ok();
}

const int _reg_okr_and_goal_tracker = testing::register_test(
    "okr_and_goal_tracker",
    "447_okr_and_goal_tracker: stub status check",
    &run_okr_and_goal_tracker);

}
