// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2078_sleep_dashboard/sleep_dashboard.hpp"

namespace {

testing::TestOutcome run_sleep_dashboard() {
    sleep_dashboard::init();
    auto s = sleep_dashboard::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_dashboard::shutdown();
    return testing::ok();
}

const int _reg_sleep_dashboard = testing::register_test(
    "sleep_dashboard",
    "2078_sleep_dashboard: stub status check",
    &run_sleep_dashboard);

}
