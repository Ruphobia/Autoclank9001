// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1810_water_bath_and_pressure_canning_guide/water_bath_and_pressure_canning_guide.hpp"

namespace {

testing::TestOutcome run_water_bath_and_pressure_canning_guide() {
    water_bath_and_pressure_canning_guide::init();
    auto s = water_bath_and_pressure_canning_guide::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    water_bath_and_pressure_canning_guide::shutdown();
    return testing::ok();
}

const int _reg_water_bath_and_pressure_canning_guide = testing::register_test(
    "water_bath_and_pressure_canning_guide",
    "1810_water_bath_and_pressure_canning_guide: stub status check",
    &run_water_bath_and_pressure_canning_guide);

}
