// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/442_time_tracker/time_tracker.hpp"

namespace {

testing::TestOutcome run_time_tracker() {
    time_tracker::init();
    auto s = time_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    time_tracker::shutdown();
    return testing::ok();
}

const int _reg_time_tracker = testing::register_test(
    "time_tracker",
    "442_time_tracker: stub status check",
    &run_time_tracker);

}
