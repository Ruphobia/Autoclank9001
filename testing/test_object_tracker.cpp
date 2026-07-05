// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1647_object_tracker/object_tracker.hpp"

namespace {

testing::TestOutcome run_object_tracker() {
    object_tracker::init();
    auto s = object_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    object_tracker::shutdown();
    return testing::ok();
}

const int _reg_object_tracker = testing::register_test(
    "object_tracker",
    "1647_object_tracker: stub status check",
    &run_object_tracker);

}
