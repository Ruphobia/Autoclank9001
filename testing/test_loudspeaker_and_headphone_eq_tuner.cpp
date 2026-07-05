// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1751_loudspeaker_and_headphone_eq_tuner/loudspeaker_and_headphone_eq_tuner.hpp"

namespace {

testing::TestOutcome run_loudspeaker_and_headphone_eq_tuner() {
    loudspeaker_and_headphone_eq_tuner::init();
    auto s = loudspeaker_and_headphone_eq_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    loudspeaker_and_headphone_eq_tuner::shutdown();
    return testing::ok();
}

const int _reg_loudspeaker_and_headphone_eq_tuner = testing::register_test(
    "loudspeaker_and_headphone_eq_tuner",
    "1751_loudspeaker_and_headphone_eq_tuner: stub status check",
    &run_loudspeaker_and_headphone_eq_tuner);

}
