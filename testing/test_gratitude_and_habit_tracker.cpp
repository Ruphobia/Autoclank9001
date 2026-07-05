// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2177_gratitude_and_habit_tracker/gratitude_and_habit_tracker.hpp"

namespace {

testing::TestOutcome run_gratitude_and_habit_tracker() {
    gratitude_and_habit_tracker::init();
    auto s = gratitude_and_habit_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gratitude_and_habit_tracker::shutdown();
    return testing::ok();
}

const int _reg_gratitude_and_habit_tracker = testing::register_test(
    "gratitude_and_habit_tracker",
    "2177_gratitude_and_habit_tracker: stub status check",
    &run_gratitude_and_habit_tracker);

}
