// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1103_burnup_and_depletion_tool/burnup_and_depletion_tool.hpp"

namespace {

testing::TestOutcome run_burnup_and_depletion_tool() {
    burnup_and_depletion_tool::init();
    auto s = burnup_and_depletion_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    burnup_and_depletion_tool::shutdown();
    return testing::ok();
}

const int _reg_burnup_and_depletion_tool = testing::register_test(
    "burnup_and_depletion_tool",
    "1103_burnup_and_depletion_tool: stub status check",
    &run_burnup_and_depletion_tool);

}
