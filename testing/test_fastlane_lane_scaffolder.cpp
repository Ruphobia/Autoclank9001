// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/171_fastlane_lane_scaffolder/fastlane_lane_scaffolder.hpp"

namespace {

testing::TestOutcome run_fastlane_lane_scaffolder() {
    fastlane_lane_scaffolder::init();
    auto s = fastlane_lane_scaffolder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fastlane_lane_scaffolder::shutdown();
    return testing::ok();
}

const int _reg_fastlane_lane_scaffolder = testing::register_test(
    "fastlane_lane_scaffolder",
    "171_fastlane_lane_scaffolder: stub status check",
    &run_fastlane_lane_scaffolder);

}
