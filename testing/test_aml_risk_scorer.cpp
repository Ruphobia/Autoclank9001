// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/610_aml_risk_scorer/aml_risk_scorer.hpp"

namespace {

testing::TestOutcome run_aml_risk_scorer() {
    aml_risk_scorer::init();
    auto s = aml_risk_scorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    aml_risk_scorer::shutdown();
    return testing::ok();
}

const int _reg_aml_risk_scorer = testing::register_test(
    "aml_risk_scorer",
    "610_aml_risk_scorer: stub status check",
    &run_aml_risk_scorer);

}
