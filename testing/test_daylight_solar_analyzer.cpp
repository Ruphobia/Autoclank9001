// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1046_daylight_solar_analyzer/daylight_solar_analyzer.hpp"

namespace {

testing::TestOutcome run_daylight_solar_analyzer() {
    daylight_solar_analyzer::init();
    auto s = daylight_solar_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    daylight_solar_analyzer::shutdown();
    return testing::ok();
}

const int _reg_daylight_solar_analyzer = testing::register_test(
    "daylight_solar_analyzer",
    "1046_daylight_solar_analyzer: stub status check",
    &run_daylight_solar_analyzer);

}
