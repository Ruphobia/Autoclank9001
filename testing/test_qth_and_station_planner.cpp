// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1197_qth_and_station_planner/qth_and_station_planner.hpp"

namespace {

testing::TestOutcome run_qth_and_station_planner() {
    qth_and_station_planner::init();
    auto s = qth_and_station_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    qth_and_station_planner::shutdown();
    return testing::ok();
}

const int _reg_qth_and_station_planner = testing::register_test(
    "qth_and_station_planner",
    "1197_qth_and_station_planner: stub status check",
    &run_qth_and_station_planner);

}
