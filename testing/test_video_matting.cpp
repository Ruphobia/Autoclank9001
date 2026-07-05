// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1643_video_matting/video_matting.hpp"

namespace {

testing::TestOutcome run_video_matting() {
    video_matting::init();
    auto s = video_matting::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    video_matting::shutdown();
    return testing::ok();
}

const int _reg_video_matting = testing::register_test(
    "video_matting",
    "1643_video_matting: stub status check",
    &run_video_matting);

}
