// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1920_rf_coordination_planner/rf_coordination_planner.hpp"

namespace {

testing::TestOutcome run_rf_coordination_planner() {
    rf_coordination_planner::init();
    auto s = rf_coordination_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rf_coordination_planner::shutdown();
    return testing::ok();
}

const int _reg_rf_coordination_planner = testing::register_test(
    "rf_coordination_planner",
    "1920_rf_coordination_planner: stub status check",
    &run_rf_coordination_planner);

}
