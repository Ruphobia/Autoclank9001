// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1095_slope_stability_analyzer/slope_stability_analyzer.hpp"

namespace {

testing::TestOutcome run_slope_stability_analyzer() {
    slope_stability_analyzer::init();
    auto s = slope_stability_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    slope_stability_analyzer::shutdown();
    return testing::ok();
}

const int _reg_slope_stability_analyzer = testing::register_test(
    "slope_stability_analyzer",
    "1095_slope_stability_analyzer: stub status check",
    &run_slope_stability_analyzer);

}
