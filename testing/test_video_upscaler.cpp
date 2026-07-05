// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1642_video_upscaler/video_upscaler.hpp"

namespace {

testing::TestOutcome run_video_upscaler() {
    video_upscaler::init();
    auto s = video_upscaler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    video_upscaler::shutdown();
    return testing::ok();
}

const int _reg_video_upscaler = testing::register_test(
    "video_upscaler",
    "1642_video_upscaler: stub status check",
    &run_video_upscaler);

}
