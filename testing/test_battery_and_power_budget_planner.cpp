#include "test_runner.hpp"
#include "../modules/864_battery_and_power_budget_planner/battery_and_power_budget_planner.hpp"

namespace {

testing::TestOutcome run_battery_and_power_budget_planner() {
    battery_and_power_budget_planner::init();
    auto s = battery_and_power_budget_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    battery_and_power_budget_planner::shutdown();
    return testing::ok();
}

const int _reg_battery_and_power_budget_planner = testing::register_test(
    "battery_and_power_budget_planner",
    "864_battery_and_power_budget_planner: stub status check",
    &run_battery_and_power_budget_planner);

}
