// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1646_speech_and_caption/speech_and_caption.hpp"

namespace {

testing::TestOutcome run_speech_and_caption() {
    speech_and_caption::init();
    auto s = speech_and_caption::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    speech_and_caption::shutdown();
    return testing::ok();
}

const int _reg_speech_and_caption = testing::register_test(
    "speech_and_caption",
    "1646_speech_and_caption: stub status check",
    &run_speech_and_caption);

}
