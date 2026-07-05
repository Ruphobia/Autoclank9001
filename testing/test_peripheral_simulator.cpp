// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/199_peripheral_simulator/peripheral_simulator.hpp"

namespace {

testing::TestOutcome run_peripheral_simulator() {
    peripheral_simulator::init();
    auto s = peripheral_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    peripheral_simulator::shutdown();
    return testing::ok();
}

const int _reg_peripheral_simulator = testing::register_test(
    "peripheral_simulator",
    "199_peripheral_simulator: stub status check",
    &run_peripheral_simulator);

}
