// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/929_cam_toolpath_generator/cam_toolpath_generator.hpp"

namespace {

testing::TestOutcome run_cam_toolpath_generator() {
    cam_toolpath_generator::init();
    auto s = cam_toolpath_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cam_toolpath_generator::shutdown();
    return testing::ok();
}

const int _reg_cam_toolpath_generator = testing::register_test(
    "cam_toolpath_generator",
    "929_cam_toolpath_generator: stub status check",
    &run_cam_toolpath_generator);

}
