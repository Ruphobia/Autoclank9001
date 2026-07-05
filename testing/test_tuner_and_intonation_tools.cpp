// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1760_tuner_and_intonation_tools/tuner_and_intonation_tools.hpp"

namespace {

testing::TestOutcome run_tuner_and_intonation_tools() {
    tuner_and_intonation_tools::init();
    auto s = tuner_and_intonation_tools::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tuner_and_intonation_tools::shutdown();
    return testing::ok();
}

const int _reg_tuner_and_intonation_tools = testing::register_test(
    "tuner_and_intonation_tools",
    "1760_tuner_and_intonation_tools: stub status check",
    &run_tuner_and_intonation_tools);

}
