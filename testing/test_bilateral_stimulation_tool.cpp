// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2188_bilateral_stimulation_tool/bilateral_stimulation_tool.hpp"

namespace {

testing::TestOutcome run_bilateral_stimulation_tool() {
    bilateral_stimulation_tool::init();
    auto s = bilateral_stimulation_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bilateral_stimulation_tool::shutdown();
    return testing::ok();
}

const int _reg_bilateral_stimulation_tool = testing::register_test(
    "bilateral_stimulation_tool",
    "2188_bilateral_stimulation_tool: stub status check",
    &run_bilateral_stimulation_tool);

}
