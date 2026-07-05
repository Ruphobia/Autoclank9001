// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1144_gravity_assist_planner/gravity_assist_planner.hpp"

namespace {

testing::TestOutcome run_gravity_assist_planner() {
    gravity_assist_planner::init();
    auto s = gravity_assist_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gravity_assist_planner::shutdown();
    return testing::ok();
}

const int _reg_gravity_assist_planner = testing::register_test(
    "gravity_assist_planner",
    "1144_gravity_assist_planner: stub status check",
    &run_gravity_assist_planner);

}
