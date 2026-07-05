// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1509_ethogram_and_behavior_coder/ethogram_and_behavior_coder.hpp"

namespace {

testing::TestOutcome run_ethogram_and_behavior_coder() {
    ethogram_and_behavior_coder::init();
    auto s = ethogram_and_behavior_coder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ethogram_and_behavior_coder::shutdown();
    return testing::ok();
}

const int _reg_ethogram_and_behavior_coder = testing::register_test(
    "ethogram_and_behavior_coder",
    "1509_ethogram_and_behavior_coder: stub status check",
    &run_ethogram_and_behavior_coder);

}
