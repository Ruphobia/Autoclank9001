// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1756_audio_fingerprinting_and_similarity/audio_fingerprinting_and_similarity.hpp"

namespace {

testing::TestOutcome run_audio_fingerprinting_and_similarity() {
    audio_fingerprinting_and_similarity::init();
    auto s = audio_fingerprinting_and_similarity::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audio_fingerprinting_and_similarity::shutdown();
    return testing::ok();
}

const int _reg_audio_fingerprinting_and_similarity = testing::register_test(
    "audio_fingerprinting_and_similarity",
    "1756_audio_fingerprinting_and_similarity: stub status check",
    &run_audio_fingerprinting_and_similarity);

}
