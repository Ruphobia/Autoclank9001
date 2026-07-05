// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1086_drilling_hydraulics_and_torque_drag/drilling_hydraulics_and_torque_drag.hpp"

namespace {

testing::TestOutcome run_drilling_hydraulics_and_torque_drag() {
    drilling_hydraulics_and_torque_drag::init();
    auto s = drilling_hydraulics_and_torque_drag::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drilling_hydraulics_and_torque_drag::shutdown();
    return testing::ok();
}

const int _reg_drilling_hydraulics_and_torque_drag = testing::register_test(
    "drilling_hydraulics_and_torque_drag",
    "1086_drilling_hydraulics_and_torque_drag: stub status check",
    &run_drilling_hydraulics_and_torque_drag);

}
