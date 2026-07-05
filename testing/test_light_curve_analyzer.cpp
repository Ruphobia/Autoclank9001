// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1487_light_curve_analyzer/light_curve_analyzer.hpp"

namespace {

testing::TestOutcome run_light_curve_analyzer() {
    light_curve_analyzer::init();
    auto s = light_curve_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    light_curve_analyzer::shutdown();
    return testing::ok();
}

const int _reg_light_curve_analyzer = testing::register_test(
    "light_curve_analyzer",
    "1487_light_curve_analyzer: stub status check",
    &run_light_curve_analyzer);

}
