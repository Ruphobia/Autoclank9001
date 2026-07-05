// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2185_reframing_assistant/reframing_assistant.hpp"

namespace {

testing::TestOutcome run_reframing_assistant() {
    reframing_assistant::init();
    auto s = reframing_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reframing_assistant::shutdown();
    return testing::ok();
}

const int _reg_reframing_assistant = testing::register_test(
    "reframing_assistant",
    "2185_reframing_assistant: stub status check",
    &run_reframing_assistant);

}
