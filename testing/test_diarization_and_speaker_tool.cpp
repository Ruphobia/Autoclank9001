// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/829_diarization_and_speaker_tool/diarization_and_speaker_tool.hpp"

namespace {

testing::TestOutcome run_diarization_and_speaker_tool() {
    diarization_and_speaker_tool::init();
    auto s = diarization_and_speaker_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    diarization_and_speaker_tool::shutdown();
    return testing::ok();
}

const int _reg_diarization_and_speaker_tool = testing::register_test(
    "diarization_and_speaker_tool",
    "829_diarization_and_speaker_tool: stub status check",
    &run_diarization_and_speaker_tool);

}
