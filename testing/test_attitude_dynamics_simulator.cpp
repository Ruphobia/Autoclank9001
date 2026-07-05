// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1139_attitude_dynamics_simulator/attitude_dynamics_simulator.hpp"

namespace {

testing::TestOutcome run_attitude_dynamics_simulator() {
    attitude_dynamics_simulator::init();
    auto s = attitude_dynamics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    attitude_dynamics_simulator::shutdown();
    return testing::ok();
}

const int _reg_attitude_dynamics_simulator = testing::register_test(
    "attitude_dynamics_simulator",
    "1139_attitude_dynamics_simulator: stub status check",
    &run_attitude_dynamics_simulator);

}
