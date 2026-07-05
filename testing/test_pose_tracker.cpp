// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1394_pose_tracker/pose_tracker.hpp"

namespace {

testing::TestOutcome run_pose_tracker() {
    pose_tracker::init();
    auto s = pose_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pose_tracker::shutdown();
    return testing::ok();
}

const int _reg_pose_tracker = testing::register_test(
    "pose_tracker",
    "1394_pose_tracker: stub status check",
    &run_pose_tracker);

}
