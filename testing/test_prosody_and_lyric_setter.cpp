// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1723_prosody_and_lyric_setter/prosody_and_lyric_setter.hpp"

namespace {

testing::TestOutcome run_prosody_and_lyric_setter() {
    prosody_and_lyric_setter::init();
    auto s = prosody_and_lyric_setter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prosody_and_lyric_setter::shutdown();
    return testing::ok();
}

const int _reg_prosody_and_lyric_setter = testing::register_test(
    "prosody_and_lyric_setter",
    "1723_prosody_and_lyric_setter: stub status check",
    &run_prosody_and_lyric_setter);

}
