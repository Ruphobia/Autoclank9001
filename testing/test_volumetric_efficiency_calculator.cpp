// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1205_volumetric_efficiency_calculator/volumetric_efficiency_calculator.hpp"

namespace {

testing::TestOutcome run_volumetric_efficiency_calculator() {
    volumetric_efficiency_calculator::init();
    auto s = volumetric_efficiency_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    volumetric_efficiency_calculator::shutdown();
    return testing::ok();
}

const int _reg_volumetric_efficiency_calculator = testing::register_test(
    "volumetric_efficiency_calculator",
    "1205_volumetric_efficiency_calculator: stub status check",
    &run_volumetric_efficiency_calculator);

}
