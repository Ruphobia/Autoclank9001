// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2197_race_track_timer_and_lap_analyzer/race_track_timer_and_lap_analyzer.hpp"

namespace {

testing::TestOutcome run_race_track_timer_and_lap_analyzer() {
    race_track_timer_and_lap_analyzer::init();
    auto s = race_track_timer_and_lap_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    race_track_timer_and_lap_analyzer::shutdown();
    return testing::ok();
}

const int _reg_race_track_timer_and_lap_analyzer = testing::register_test(
    "race_track_timer_and_lap_analyzer",
    "2197_race_track_timer_and_lap_analyzer: stub status check",
    &run_race_track_timer_and_lap_analyzer);

}
