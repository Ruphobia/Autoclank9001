// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1880_attribution_assistant/attribution_assistant.hpp"

namespace {

testing::TestOutcome run_attribution_assistant() {
    attribution_assistant::init();
    auto s = attribution_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    attribution_assistant::shutdown();
    return testing::ok();
}

const int _reg_attribution_assistant = testing::register_test(
    "attribution_assistant",
    "1880_attribution_assistant: stub status check",
    &run_attribution_assistant);

}
