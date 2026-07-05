// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1006_thermal_expansion_mismatch_tool/thermal_expansion_mismatch_tool.hpp"

namespace {

testing::TestOutcome run_thermal_expansion_mismatch_tool() {
    thermal_expansion_mismatch_tool::init();
    auto s = thermal_expansion_mismatch_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    thermal_expansion_mismatch_tool::shutdown();
    return testing::ok();
}

const int _reg_thermal_expansion_mismatch_tool = testing::register_test(
    "thermal_expansion_mismatch_tool",
    "1006_thermal_expansion_mismatch_tool: stub status check",
    &run_thermal_expansion_mismatch_tool);

}
