// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/816_speech_synthesizer/speech_synthesizer.hpp"

namespace {

testing::TestOutcome run_speech_synthesizer() {
    speech_synthesizer::init();
    auto s = speech_synthesizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    speech_synthesizer::shutdown();
    return testing::ok();
}

const int _reg_speech_synthesizer = testing::register_test(
    "speech_synthesizer",
    "816_speech_synthesizer: stub status check",
    &run_speech_synthesizer);

}
