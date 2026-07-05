// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2061_climb_grade_converter/climb_grade_converter.hpp"

namespace {

testing::TestOutcome run_climb_grade_converter() {
    climb_grade_converter::init();
    auto s = climb_grade_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    climb_grade_converter::shutdown();
    return testing::ok();
}

const int _reg_climb_grade_converter = testing::register_test(
    "climb_grade_converter",
    "2061_climb_grade_converter: stub status check",
    &run_climb_grade_converter);

}
