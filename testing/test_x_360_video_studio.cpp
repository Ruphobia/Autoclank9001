// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/315_x_360_video_studio/x_360_video_studio.hpp"

namespace {

testing::TestOutcome run_x_360_video_studio() {
    x_360_video_studio::init();
    auto s = x_360_video_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    x_360_video_studio::shutdown();
    return testing::ok();
}

const int _reg_x_360_video_studio = testing::register_test(
    "x_360_video_studio",
    "315_x_360_video_studio: stub status check",
    &run_x_360_video_studio);

}
