// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/448_risk_register/risk_register.hpp"

namespace {

testing::TestOutcome run_risk_register() {
    risk_register::init();
    auto s = risk_register::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    risk_register::shutdown();
    return testing::ok();
}

const int _reg_risk_register = testing::register_test(
    "risk_register",
    "448_risk_register: stub status check",
    &run_risk_register);

}
