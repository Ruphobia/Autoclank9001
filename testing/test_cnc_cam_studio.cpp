// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/975_cnc_cam_studio/cnc_cam_studio.hpp"

namespace {

testing::TestOutcome run_cnc_cam_studio() {
    cnc_cam_studio::init();
    auto s = cnc_cam_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cnc_cam_studio::shutdown();
    return testing::ok();
}

const int _reg_cnc_cam_studio = testing::register_test(
    "cnc_cam_studio",
    "975_cnc_cam_studio: stub status check",
    &run_cnc_cam_studio);

}
