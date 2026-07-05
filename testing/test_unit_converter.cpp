// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/893_unit_converter/unit_converter.hpp"

namespace {

testing::TestOutcome run_unit_converter() {
    unit_converter::init();
    auto s = unit_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    unit_converter::shutdown();
    return testing::ok();
}

const int _reg_unit_converter = testing::register_test(
    "unit_converter",
    "893_unit_converter: stub status check",
    &run_unit_converter);

}
