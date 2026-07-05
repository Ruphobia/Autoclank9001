// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1068_water_quality_kinetics/water_quality_kinetics.hpp"

namespace {

testing::TestOutcome run_water_quality_kinetics() {
    water_quality_kinetics::init();
    auto s = water_quality_kinetics::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    water_quality_kinetics::shutdown();
    return testing::ok();
}

const int _reg_water_quality_kinetics = testing::register_test(
    "water_quality_kinetics",
    "1068_water_quality_kinetics: stub status check",
    &run_water_quality_kinetics);

}
