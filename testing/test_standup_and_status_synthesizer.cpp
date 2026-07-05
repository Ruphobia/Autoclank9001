// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/450_standup_and_status_synthesizer/standup_and_status_synthesizer.hpp"

namespace {

testing::TestOutcome run_standup_and_status_synthesizer() {
    standup_and_status_synthesizer::init();
    auto s = standup_and_status_synthesizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    standup_and_status_synthesizer::shutdown();
    return testing::ok();
}

const int _reg_standup_and_status_synthesizer = testing::register_test(
    "standup_and_status_synthesizer",
    "450_standup_and_status_synthesizer: stub status check",
    &run_standup_and_status_synthesizer);

}
