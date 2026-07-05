// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1286_wind_turbine_power_curve_studio/wind_turbine_power_curve_studio.hpp"

namespace {

testing::TestOutcome run_wind_turbine_power_curve_studio() {
    wind_turbine_power_curve_studio::init();
    auto s = wind_turbine_power_curve_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wind_turbine_power_curve_studio::shutdown();
    return testing::ok();
}

const int _reg_wind_turbine_power_curve_studio = testing::register_test(
    "wind_turbine_power_curve_studio",
    "1286_wind_turbine_power_curve_studio: stub status check",
    &run_wind_turbine_power_curve_studio);

}
