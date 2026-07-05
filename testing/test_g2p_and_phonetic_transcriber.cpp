// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/822_g2p_and_phonetic_transcriber/g2p_and_phonetic_transcriber.hpp"

namespace {

testing::TestOutcome run_g2p_and_phonetic_transcriber() {
    g2p_and_phonetic_transcriber::init();
    auto s = g2p_and_phonetic_transcriber::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    g2p_and_phonetic_transcriber::shutdown();
    return testing::ok();
}

const int _reg_g2p_and_phonetic_transcriber = testing::register_test(
    "g2p_and_phonetic_transcriber",
    "822_g2p_and_phonetic_transcriber: stub status check",
    &run_g2p_and_phonetic_transcriber);

}
