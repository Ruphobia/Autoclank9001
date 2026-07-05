// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1593_lidar_point_cloud_viewer/lidar_point_cloud_viewer.hpp"

namespace {

testing::TestOutcome run_lidar_point_cloud_viewer() {
    lidar_point_cloud_viewer::init();
    auto s = lidar_point_cloud_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lidar_point_cloud_viewer::shutdown();
    return testing::ok();
}

const int _reg_lidar_point_cloud_viewer = testing::register_test(
    "lidar_point_cloud_viewer",
    "1593_lidar_point_cloud_viewer: stub status check",
    &run_lidar_point_cloud_viewer);

}
