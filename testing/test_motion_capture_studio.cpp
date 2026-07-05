// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1256_motion_capture_studio/motion_capture_studio.hpp"

namespace {

testing::TestOutcome run_motion_capture_studio() {
    motion_capture_studio::init();
    auto s = motion_capture_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    motion_capture_studio::shutdown();
    return testing::ok();
}

const int _reg_motion_capture_studio = testing::register_test(
    "motion_capture_studio",
    "1256_motion_capture_studio: stub status check",
    &run_motion_capture_studio);

}
