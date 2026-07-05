// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1119_engineering_unit_converter/engineering_unit_converter.hpp"

namespace {

testing::TestOutcome run_engineering_unit_converter() {
    engineering_unit_converter::init();
    auto s = engineering_unit_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    engineering_unit_converter::shutdown();
    return testing::ok();
}

const int _reg_engineering_unit_converter = testing::register_test(
    "engineering_unit_converter",
    "1119_engineering_unit_converter: stub status check",
    &run_engineering_unit_converter);

}
