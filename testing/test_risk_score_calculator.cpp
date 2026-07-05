// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1411_risk_score_calculator/risk_score_calculator.hpp"

namespace {

testing::TestOutcome run_risk_score_calculator() {
    risk_score_calculator::init();
    auto s = risk_score_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    risk_score_calculator::shutdown();
    return testing::ok();
}

const int _reg_risk_score_calculator = testing::register_test(
    "risk_score_calculator",
    "1411_risk_score_calculator: stub status check",
    &run_risk_score_calculator);

}
