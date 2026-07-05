// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1644_text_to_video/text_to_video.hpp"

namespace {

testing::TestOutcome run_text_to_video() {
    text_to_video::init();
    auto s = text_to_video::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    text_to_video::shutdown();
    return testing::ok();
}

const int _reg_text_to_video = testing::register_test(
    "text_to_video",
    "1644_text_to_video: stub status check",
    &run_text_to_video);

}
