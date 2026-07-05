// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/437_sprint_planner/sprint_planner.hpp"

namespace {

testing::TestOutcome run_sprint_planner() {
    sprint_planner::init();
    auto s = sprint_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sprint_planner::shutdown();
    return testing::ok();
}

const int _reg_sprint_planner = testing::register_test(
    "sprint_planner",
    "437_sprint_planner: stub status check",
    &run_sprint_planner);

}
