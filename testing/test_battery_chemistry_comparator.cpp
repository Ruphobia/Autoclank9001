// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1281_battery_chemistry_comparator/battery_chemistry_comparator.hpp"

namespace {

testing::TestOutcome run_battery_chemistry_comparator() {
    battery_chemistry_comparator::init();
    auto s = battery_chemistry_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    battery_chemistry_comparator::shutdown();
    return testing::ok();
}

const int _reg_battery_chemistry_comparator = testing::register_test(
    "battery_chemistry_comparator",
    "1281_battery_chemistry_comparator: stub status check",
    &run_battery_chemistry_comparator);

}
