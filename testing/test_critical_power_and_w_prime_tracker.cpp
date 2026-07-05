// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2080_critical_power_and_w_prime_tracker/critical_power_and_w_prime_tracker.hpp"

namespace {

testing::TestOutcome run_critical_power_and_w_prime_tracker() {
    critical_power_and_w_prime_tracker::init();
    auto s = critical_power_and_w_prime_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    critical_power_and_w_prime_tracker::shutdown();
    return testing::ok();
}

const int _reg_critical_power_and_w_prime_tracker = testing::register_test(
    "critical_power_and_w_prime_tracker",
    "2080_critical_power_and_w_prime_tracker: stub status check",
    &run_critical_power_and_w_prime_tracker);

}
