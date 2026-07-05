// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2053_tide_and_current_charts/tide_and_current_charts.hpp"

namespace {

testing::TestOutcome run_tide_and_current_charts() {
    tide_and_current_charts::init();
    auto s = tide_and_current_charts::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tide_and_current_charts::shutdown();
    return testing::ok();
}

const int _reg_tide_and_current_charts = testing::register_test(
    "tide_and_current_charts",
    "2053_tide_and_current_charts: stub status check",
    &run_tide_and_current_charts);

}
