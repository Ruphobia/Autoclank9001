// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1559_coral_dhw_tracker/coral_dhw_tracker.hpp"

namespace {

testing::TestOutcome run_coral_dhw_tracker() {
    coral_dhw_tracker::init();
    auto s = coral_dhw_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    coral_dhw_tracker::shutdown();
    return testing::ok();
}

const int _reg_coral_dhw_tracker = testing::register_test(
    "coral_dhw_tracker",
    "1559_coral_dhw_tracker: stub status check",
    &run_coral_dhw_tracker);

}
