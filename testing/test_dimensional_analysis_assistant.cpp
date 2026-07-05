// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/916_dimensional_analysis_assistant/dimensional_analysis_assistant.hpp"

namespace {

testing::TestOutcome run_dimensional_analysis_assistant() {
    dimensional_analysis_assistant::init();
    auto s = dimensional_analysis_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dimensional_analysis_assistant::shutdown();
    return testing::ok();
}

const int _reg_dimensional_analysis_assistant = testing::register_test(
    "dimensional_analysis_assistant",
    "916_dimensional_analysis_assistant: stub status check",
    &run_dimensional_analysis_assistant);

}
