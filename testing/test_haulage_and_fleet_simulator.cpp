// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1097_haulage_and_fleet_simulator/haulage_and_fleet_simulator.hpp"

namespace {

testing::TestOutcome run_haulage_and_fleet_simulator() {
    haulage_and_fleet_simulator::init();
    auto s = haulage_and_fleet_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    haulage_and_fleet_simulator::shutdown();
    return testing::ok();
}

const int _reg_haulage_and_fleet_simulator = testing::register_test(
    "haulage_and_fleet_simulator",
    "1097_haulage_and_fleet_simulator: stub status check",
    &run_haulage_and_fleet_simulator);

}
