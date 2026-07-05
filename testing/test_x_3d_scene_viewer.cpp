// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1253_x_3d_scene_viewer/x_3d_scene_viewer.hpp"

namespace {

testing::TestOutcome run_x_3d_scene_viewer() {
    x_3d_scene_viewer::init();
    auto s = x_3d_scene_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    x_3d_scene_viewer::shutdown();
    return testing::ok();
}

const int _reg_x_3d_scene_viewer = testing::register_test(
    "x_3d_scene_viewer",
    "1253_x_3d_scene_viewer: stub status check",
    &run_x_3d_scene_viewer);

}
