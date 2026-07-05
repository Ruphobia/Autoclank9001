// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1866_pigment_mixer/pigment_mixer.hpp"

namespace {

testing::TestOutcome run_pigment_mixer() {
    pigment_mixer::init();
    auto s = pigment_mixer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pigment_mixer::shutdown();
    return testing::ok();
}

const int _reg_pigment_mixer = testing::register_test(
    "pigment_mixer",
    "1866_pigment_mixer: stub status check",
    &run_pigment_mixer);

}
