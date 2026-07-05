// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/586_privacy_policy_generator/privacy_policy_generator.hpp"

namespace {

testing::TestOutcome run_privacy_policy_generator() {
    privacy_policy_generator::init();
    auto s = privacy_policy_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    privacy_policy_generator::shutdown();
    return testing::ok();
}

const int _reg_privacy_policy_generator = testing::register_test(
    "privacy_policy_generator",
    "586_privacy_policy_generator: stub status check",
    &run_privacy_policy_generator);

}
