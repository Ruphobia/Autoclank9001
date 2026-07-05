// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1217_gps_lap_and_sector_analyzer/gps_lap_and_sector_analyzer.hpp"

namespace {

testing::TestOutcome run_gps_lap_and_sector_analyzer() {
    gps_lap_and_sector_analyzer::init();
    auto s = gps_lap_and_sector_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gps_lap_and_sector_analyzer::shutdown();
    return testing::ok();
}

const int _reg_gps_lap_and_sector_analyzer = testing::register_test(
    "gps_lap_and_sector_analyzer",
    "1217_gps_lap_and_sector_analyzer: stub status check",
    &run_gps_lap_and_sector_analyzer);

}
