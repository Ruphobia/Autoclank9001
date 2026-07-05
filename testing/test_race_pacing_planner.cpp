// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2081_race_pacing_planner/race_pacing_planner.hpp"

namespace {

testing::TestOutcome run_race_pacing_planner() {
    race_pacing_planner::init();
    auto s = race_pacing_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    race_pacing_planner::shutdown();
    return testing::ok();
}

const int _reg_race_pacing_planner = testing::register_test(
    "race_pacing_planner",
    "2081_race_pacing_planner: stub status check",
    &run_race_pacing_planner);

}
