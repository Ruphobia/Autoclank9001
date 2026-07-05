// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1230_brake_pad_rotor_reference/brake_pad_rotor_reference.hpp"

namespace {

testing::TestOutcome run_brake_pad_rotor_reference() {
    brake_pad_rotor_reference::init();
    auto s = brake_pad_rotor_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    brake_pad_rotor_reference::shutdown();
    return testing::ok();
}

const int _reg_brake_pad_rotor_reference = testing::register_test(
    "brake_pad_rotor_reference",
    "1230_brake_pad_rotor_reference: stub status check",
    &run_brake_pad_rotor_reference);

}
