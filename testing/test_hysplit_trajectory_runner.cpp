// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1465_hysplit_trajectory_runner/hysplit_trajectory_runner.hpp"

namespace {

testing::TestOutcome run_hysplit_trajectory_runner() {
    hysplit_trajectory_runner::init();
    auto s = hysplit_trajectory_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hysplit_trajectory_runner::shutdown();
    return testing::ok();
}

const int _reg_hysplit_trajectory_runner = testing::register_test(
    "hysplit_trajectory_runner",
    "1465_hysplit_trajectory_runner: stub status check",
    &run_hysplit_trajectory_runner);

}
