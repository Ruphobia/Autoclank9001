// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/364_password_recovery_driver/password_recovery_driver.hpp"

namespace {

testing::TestOutcome run_password_recovery_driver() {
    password_recovery_driver::init();
    auto s = password_recovery_driver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    password_recovery_driver::shutdown();
    return testing::ok();
}

const int _reg_password_recovery_driver = testing::register_test(
    "password_recovery_driver",
    "364_password_recovery_driver: stub status check",
    &run_password_recovery_driver);

}
