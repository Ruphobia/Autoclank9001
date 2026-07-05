// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1908_style_reference_generator/style_reference_generator.hpp"

namespace {

testing::TestOutcome run_style_reference_generator() {
    style_reference_generator::init();
    auto s = style_reference_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    style_reference_generator::shutdown();
    return testing::ok();
}

const int _reg_style_reference_generator = testing::register_test(
    "style_reference_generator",
    "1908_style_reference_generator: stub status check",
    &run_style_reference_generator);

}
