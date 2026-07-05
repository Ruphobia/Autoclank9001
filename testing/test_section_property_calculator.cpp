// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1055_section_property_calculator/section_property_calculator.hpp"

namespace {

testing::TestOutcome run_section_property_calculator() {
    section_property_calculator::init();
    auto s = section_property_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    section_property_calculator::shutdown();
    return testing::ok();
}

const int _reg_section_property_calculator = testing::register_test(
    "section_property_calculator",
    "1055_section_property_calculator: stub status check",
    &run_section_property_calculator);

}
