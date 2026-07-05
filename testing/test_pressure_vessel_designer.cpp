// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1110_pressure_vessel_designer/pressure_vessel_designer.hpp"

namespace {

testing::TestOutcome run_pressure_vessel_designer() {
    pressure_vessel_designer::init();
    auto s = pressure_vessel_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pressure_vessel_designer::shutdown();
    return testing::ok();
}

const int _reg_pressure_vessel_designer = testing::register_test(
    "pressure_vessel_designer",
    "1110_pressure_vessel_designer: stub status check",
    &run_pressure_vessel_designer);

}
