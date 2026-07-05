// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/960_pump_and_fan_sizer/pump_and_fan_sizer.hpp"

namespace {

testing::TestOutcome run_pump_and_fan_sizer() {
    pump_and_fan_sizer::init();
    auto s = pump_and_fan_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pump_and_fan_sizer::shutdown();
    return testing::ok();
}

const int _reg_pump_and_fan_sizer = testing::register_test(
    "pump_and_fan_sizer",
    "960_pump_and_fan_sizer: stub status check",
    &run_pump_and_fan_sizer);

}
