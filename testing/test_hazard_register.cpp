// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/469_hazard_register/hazard_register.hpp"

namespace {

testing::TestOutcome run_hazard_register() {
    hazard_register::init();
    auto s = hazard_register::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hazard_register::shutdown();
    return testing::ok();
}

const int _reg_hazard_register = testing::register_test(
    "hazard_register",
    "469_hazard_register: stub status check",
    &run_hazard_register);

}
