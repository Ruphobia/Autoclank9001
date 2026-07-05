// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1178_award_tracker/award_tracker.hpp"

namespace {

testing::TestOutcome run_award_tracker() {
    award_tracker::init();
    auto s = award_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    award_tracker::shutdown();
    return testing::ok();
}

const int _reg_award_tracker = testing::register_test(
    "award_tracker",
    "1178_award_tracker: stub status check",
    &run_award_tracker);

}
