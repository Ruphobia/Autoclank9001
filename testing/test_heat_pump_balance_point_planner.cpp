// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2027_heat_pump_balance_point_planner/heat_pump_balance_point_planner.hpp"

namespace {

testing::TestOutcome run_heat_pump_balance_point_planner() {
    heat_pump_balance_point_planner::init();
    auto s = heat_pump_balance_point_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    heat_pump_balance_point_planner::shutdown();
    return testing::ok();
}

const int _reg_heat_pump_balance_point_planner = testing::register_test(
    "heat_pump_balance_point_planner",
    "2027_heat_pump_balance_point_planner: stub status check",
    &run_heat_pump_balance_point_planner);

}
