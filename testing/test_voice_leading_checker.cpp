// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1708_voice_leading_checker/voice_leading_checker.hpp"

namespace {

testing::TestOutcome run_voice_leading_checker() {
    voice_leading_checker::init();
    auto s = voice_leading_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    voice_leading_checker::shutdown();
    return testing::ok();
}

const int _reg_voice_leading_checker = testing::register_test(
    "voice_leading_checker",
    "1708_voice_leading_checker: stub status check",
    &run_voice_leading_checker);

}
