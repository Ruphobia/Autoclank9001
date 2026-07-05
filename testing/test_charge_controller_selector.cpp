// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1282_charge_controller_selector/charge_controller_selector.hpp"

namespace {

testing::TestOutcome run_charge_controller_selector() {
    charge_controller_selector::init();
    auto s = charge_controller_selector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    charge_controller_selector::shutdown();
    return testing::ok();
}

const int _reg_charge_controller_selector = testing::register_test(
    "charge_controller_selector",
    "1282_charge_controller_selector: stub status check",
    &run_charge_controller_selector);

}
