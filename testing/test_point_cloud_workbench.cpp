// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1254_point_cloud_workbench/point_cloud_workbench.hpp"

namespace {

testing::TestOutcome run_point_cloud_workbench() {
    point_cloud_workbench::init();
    auto s = point_cloud_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    point_cloud_workbench::shutdown();
    return testing::ok();
}

const int _reg_point_cloud_workbench = testing::register_test(
    "point_cloud_workbench",
    "1254_point_cloud_workbench: stub status check",
    &run_point_cloud_workbench);

}
