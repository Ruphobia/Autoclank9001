// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1133_delta_v_budget_planner/delta_v_budget_planner.hpp"

namespace {

testing::TestOutcome run_delta_v_budget_planner() {
    delta_v_budget_planner::init();
    auto s = delta_v_budget_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    delta_v_budget_planner::shutdown();
    return testing::ok();
}

const int _reg_delta_v_budget_planner = testing::register_test(
    "delta_v_budget_planner",
    "1133_delta_v_budget_planner: stub status check",
    &run_delta_v_budget_planner);

}
