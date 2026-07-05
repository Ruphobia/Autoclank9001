// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/451_resource_and_capacity_planner/resource_and_capacity_planner.hpp"

namespace {

testing::TestOutcome run_resource_and_capacity_planner() {
    resource_and_capacity_planner::init();
    auto s = resource_and_capacity_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    resource_and_capacity_planner::shutdown();
    return testing::ok();
}

const int _reg_resource_and_capacity_planner = testing::register_test(
    "resource_and_capacity_planner",
    "451_resource_and_capacity_planner: stub status check",
    &run_resource_and_capacity_planner);

}
