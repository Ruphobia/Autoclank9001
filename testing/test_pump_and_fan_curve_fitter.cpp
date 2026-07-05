// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1021_pump_and_fan_curve_fitter/pump_and_fan_curve_fitter.hpp"

namespace {

testing::TestOutcome run_pump_and_fan_curve_fitter() {
    pump_and_fan_curve_fitter::init();
    auto s = pump_and_fan_curve_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pump_and_fan_curve_fitter::shutdown();
    return testing::ok();
}

const int _reg_pump_and_fan_curve_fitter = testing::register_test(
    "pump_and_fan_curve_fitter",
    "1021_pump_and_fan_curve_fitter: stub status check",
    &run_pump_and_fan_curve_fitter);

}
