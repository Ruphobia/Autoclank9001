// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2105_sail_trim_assistant/sail_trim_assistant.hpp"

namespace {

testing::TestOutcome run_sail_trim_assistant() {
    sail_trim_assistant::init();
    auto s = sail_trim_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sail_trim_assistant::shutdown();
    return testing::ok();
}

const int _reg_sail_trim_assistant = testing::register_test(
    "sail_trim_assistant",
    "2105_sail_trim_assistant: stub status check",
    &run_sail_trim_assistant);

}
