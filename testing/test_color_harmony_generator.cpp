// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1862_color_harmony_generator/color_harmony_generator.hpp"

namespace {

testing::TestOutcome run_color_harmony_generator() {
    color_harmony_generator::init();
    auto s = color_harmony_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_harmony_generator::shutdown();
    return testing::ok();
}

const int _reg_color_harmony_generator = testing::register_test(
    "color_harmony_generator",
    "1862_color_harmony_generator: stub status check",
    &run_color_harmony_generator);

}
