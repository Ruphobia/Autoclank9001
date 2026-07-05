// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1025_pneumatic_actuator_sizer/pneumatic_actuator_sizer.hpp"

namespace {

testing::TestOutcome run_pneumatic_actuator_sizer() {
    pneumatic_actuator_sizer::init();
    auto s = pneumatic_actuator_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pneumatic_actuator_sizer::shutdown();
    return testing::ok();
}

const int _reg_pneumatic_actuator_sizer = testing::register_test(
    "pneumatic_actuator_sizer",
    "1025_pneumatic_actuator_sizer: stub status check",
    &run_pneumatic_actuator_sizer);

}
