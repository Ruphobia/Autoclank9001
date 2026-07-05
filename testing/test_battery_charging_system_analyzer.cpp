// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1242_battery_charging_system_analyzer/battery_charging_system_analyzer.hpp"

namespace {

testing::TestOutcome run_battery_charging_system_analyzer() {
    battery_charging_system_analyzer::init();
    auto s = battery_charging_system_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    battery_charging_system_analyzer::shutdown();
    return testing::ok();
}

const int _reg_battery_charging_system_analyzer = testing::register_test(
    "battery_charging_system_analyzer",
    "1242_battery_charging_system_analyzer: stub status check",
    &run_battery_charging_system_analyzer);

}
