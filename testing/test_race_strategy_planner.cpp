// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1218_race_strategy_planner/race_strategy_planner.hpp"

namespace {

testing::TestOutcome run_race_strategy_planner() {
    race_strategy_planner::init();
    auto s = race_strategy_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    race_strategy_planner::shutdown();
    return testing::ok();
}

const int _reg_race_strategy_planner = testing::register_test(
    "race_strategy_planner",
    "1218_race_strategy_planner: stub status check",
    &run_race_strategy_planner);

}
