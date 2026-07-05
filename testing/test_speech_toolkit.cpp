// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1755_speech_toolkit/speech_toolkit.hpp"

namespace {

testing::TestOutcome run_speech_toolkit() {
    speech_toolkit::init();
    auto s = speech_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    speech_toolkit::shutdown();
    return testing::ok();
}

const int _reg_speech_toolkit = testing::register_test(
    "speech_toolkit",
    "1755_speech_toolkit: stub status check",
    &run_speech_toolkit);

}
