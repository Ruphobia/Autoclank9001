// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1737_pitch_and_time_tools/pitch_and_time_tools.hpp"

namespace {

testing::TestOutcome run_pitch_and_time_tools() {
    pitch_and_time_tools::init();
    auto s = pitch_and_time_tools::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pitch_and_time_tools::shutdown();
    return testing::ok();
}

const int _reg_pitch_and_time_tools = testing::register_test(
    "pitch_and_time_tools",
    "1737_pitch_and_time_tools: stub status check",
    &run_pitch_and_time_tools);

}
