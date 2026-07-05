// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2196_fpv_setup_and_video_link/fpv_setup_and_video_link.hpp"

namespace {

testing::TestOutcome run_fpv_setup_and_video_link() {
    fpv_setup_and_video_link::init();
    auto s = fpv_setup_and_video_link::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fpv_setup_and_video_link::shutdown();
    return testing::ok();
}

const int _reg_fpv_setup_and_video_link = testing::register_test(
    "fpv_setup_and_video_link",
    "2196_fpv_setup_and_video_link: stub status check",
    &run_fpv_setup_and_video_link);

}
