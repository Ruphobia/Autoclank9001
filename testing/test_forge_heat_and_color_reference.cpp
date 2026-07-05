// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1842_forge_heat_and_color_reference/forge_heat_and_color_reference.hpp"

namespace {

testing::TestOutcome run_forge_heat_and_color_reference() {
    forge_heat_and_color_reference::init();
    auto s = forge_heat_and_color_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    forge_heat_and_color_reference::shutdown();
    return testing::ok();
}

const int _reg_forge_heat_and_color_reference = testing::register_test(
    "forge_heat_and_color_reference",
    "1842_forge_heat_and_color_reference: stub status check",
    &run_forge_heat_and_color_reference);

}
