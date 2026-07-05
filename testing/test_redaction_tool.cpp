// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/594_redaction_tool/redaction_tool.hpp"

namespace {

testing::TestOutcome run_redaction_tool() {
    redaction_tool::init();
    auto s = redaction_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    redaction_tool::shutdown();
    return testing::ok();
}

const int _reg_redaction_tool = testing::register_test(
    "redaction_tool",
    "594_redaction_tool: stub status check",
    &run_redaction_tool);

}
