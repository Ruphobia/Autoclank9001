// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1475_sky_chart/sky_chart.hpp"

namespace {

testing::TestOutcome run_sky_chart() {
    sky_chart::init();
    auto s = sky_chart::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sky_chart::shutdown();
    return testing::ok();
}

const int _reg_sky_chart = testing::register_test(
    "sky_chart",
    "1475_sky_chart: stub status check",
    &run_sky_chart);

}
