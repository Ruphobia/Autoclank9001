// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1779_colorway_palette_builder/colorway_palette_builder.hpp"

namespace {

testing::TestOutcome run_colorway_palette_builder() {
    colorway_palette_builder::init();
    auto s = colorway_palette_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    colorway_palette_builder::shutdown();
    return testing::ok();
}

const int _reg_colorway_palette_builder = testing::register_test(
    "colorway_palette_builder",
    "1779_colorway_palette_builder: stub status check",
    &run_colorway_palette_builder);

}
