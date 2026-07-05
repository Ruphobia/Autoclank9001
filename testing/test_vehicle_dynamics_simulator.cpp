// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1219_vehicle_dynamics_simulator/vehicle_dynamics_simulator.hpp"

namespace {

testing::TestOutcome run_vehicle_dynamics_simulator() {
    vehicle_dynamics_simulator::init();
    auto s = vehicle_dynamics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vehicle_dynamics_simulator::shutdown();
    return testing::ok();
}

const int _reg_vehicle_dynamics_simulator = testing::register_test(
    "vehicle_dynamics_simulator",
    "1219_vehicle_dynamics_simulator: stub status check",
    &run_vehicle_dynamics_simulator);

}
