// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1084_decline_curve_analyzer/decline_curve_analyzer.hpp"

namespace {

testing::TestOutcome run_decline_curve_analyzer() {
    decline_curve_analyzer::init();
    auto s = decline_curve_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    decline_curve_analyzer::shutdown();
    return testing::ok();
}

const int _reg_decline_curve_analyzer = testing::register_test(
    "decline_curve_analyzer",
    "1084_decline_curve_analyzer: stub status check",
    &run_decline_curve_analyzer);

}
