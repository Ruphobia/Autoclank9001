// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/316_hand_pose_authoring/hand_pose_authoring.hpp"

namespace {

testing::TestOutcome run_hand_pose_authoring() {
    hand_pose_authoring::init();
    auto s = hand_pose_authoring::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hand_pose_authoring::shutdown();
    return testing::ok();
}

const int _reg_hand_pose_authoring = testing::register_test(
    "hand_pose_authoring",
    "316_hand_pose_authoring: stub status check",
    &run_hand_pose_authoring);

}
