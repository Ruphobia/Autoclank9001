// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1634_format_converter/format_converter.hpp"

namespace {

testing::TestOutcome run_format_converter() {
    format_converter::init();
    auto s = format_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    format_converter::shutdown();
    return testing::ok();
}

const int _reg_format_converter = testing::register_test(
    "format_converter",
    "1634_format_converter: stub status check",
    &run_format_converter);

}
