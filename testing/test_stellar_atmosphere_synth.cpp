// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1497_stellar_atmosphere_synth/stellar_atmosphere_synth.hpp"

namespace {

testing::TestOutcome run_stellar_atmosphere_synth() {
    stellar_atmosphere_synth::init();
    auto s = stellar_atmosphere_synth::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stellar_atmosphere_synth::shutdown();
    return testing::ok();
}

const int _reg_stellar_atmosphere_synth = testing::register_test(
    "stellar_atmosphere_synth",
    "1497_stellar_atmosphere_synth: stub status check",
    &run_stellar_atmosphere_synth);

}
