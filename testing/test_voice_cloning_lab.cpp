// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/821_voice_cloning_lab/voice_cloning_lab.hpp"

namespace {

testing::TestOutcome run_voice_cloning_lab() {
    voice_cloning_lab::init();
    auto s = voice_cloning_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    voice_cloning_lab::shutdown();
    return testing::ok();
}

const int _reg_voice_cloning_lab = testing::register_test(
    "voice_cloning_lab",
    "821_voice_cloning_lab: stub status check",
    &run_voice_cloning_lab);

}
