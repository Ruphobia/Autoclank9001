// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/446_milestone_and_release_tracker/milestone_and_release_tracker.hpp"

namespace {

testing::TestOutcome run_milestone_and_release_tracker() {
    milestone_and_release_tracker::init();
    auto s = milestone_and_release_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    milestone_and_release_tracker::shutdown();
    return testing::ok();
}

const int _reg_milestone_and_release_tracker = testing::register_test(
    "milestone_and_release_tracker",
    "446_milestone_and_release_tracker: stub status check",
    &run_milestone_and_release_tracker);

}
