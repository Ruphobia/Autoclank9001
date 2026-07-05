// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1523_geochronology_calculator/geochronology_calculator.hpp"

namespace {

testing::TestOutcome run_geochronology_calculator() {
    geochronology_calculator::init();
    auto s = geochronology_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geochronology_calculator::shutdown();
    return testing::ok();
}

const int _reg_geochronology_calculator = testing::register_test(
    "geochronology_calculator",
    "1523_geochronology_calculator: stub status check",
    &run_geochronology_calculator);

}
