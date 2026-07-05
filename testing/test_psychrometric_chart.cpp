// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1043_psychrometric_chart/psychrometric_chart.hpp"

namespace {

testing::TestOutcome run_psychrometric_chart() {
    psychrometric_chart::init();
    auto s = psychrometric_chart::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    psychrometric_chart::shutdown();
    return testing::ok();
}

const int _reg_psychrometric_chart = testing::register_test(
    "psychrometric_chart",
    "1043_psychrometric_chart: stub status check",
    &run_psychrometric_chart);

}
