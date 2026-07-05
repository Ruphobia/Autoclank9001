// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1129_element_set_converter/element_set_converter.hpp"

namespace {

testing::TestOutcome run_element_set_converter() {
    element_set_converter::init();
    auto s = element_set_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    element_set_converter::shutdown();
    return testing::ok();
}

const int _reg_element_set_converter = testing::register_test(
    "element_set_converter",
    "1129_element_set_converter: stub status check",
    &run_element_set_converter);

}
