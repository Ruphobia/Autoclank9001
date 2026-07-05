// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1629_pose_and_depth_extractor/pose_and_depth_extractor.hpp"

namespace {

testing::TestOutcome run_pose_and_depth_extractor() {
    pose_and_depth_extractor::init();
    auto s = pose_and_depth_extractor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pose_and_depth_extractor::shutdown();
    return testing::ok();
}

const int _reg_pose_and_depth_extractor = testing::register_test(
    "pose_and_depth_extractor",
    "1629_pose_and_depth_extractor: stub status check",
    &run_pose_and_depth_extractor);

}
