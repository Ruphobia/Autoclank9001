// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1725_audio_recorder/audio_recorder.hpp"

namespace {

testing::TestOutcome run_audio_recorder() {
    audio_recorder::init();
    auto s = audio_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audio_recorder::shutdown();
    return testing::ok();
}

const int _reg_audio_recorder = testing::register_test(
    "audio_recorder",
    "1725_audio_recorder: stub status check",
    &run_audio_recorder);

}
