// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1609_frost_risk_predictor/frost_risk_predictor.hpp"

namespace {

testing::TestOutcome run_frost_risk_predictor() {
    frost_risk_predictor::init();
    auto s = frost_risk_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frost_risk_predictor::shutdown();
    return testing::ok();
}

const int _reg_frost_risk_predictor = testing::register_test(
    "frost_risk_predictor",
    "1609_frost_risk_predictor: stub status check",
    &run_frost_risk_predictor);

}
