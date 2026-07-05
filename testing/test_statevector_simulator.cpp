// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/333_statevector_simulator/statevector_simulator.hpp"

namespace {

testing::TestOutcome run_statevector_simulator() {
    statevector_simulator::init();
    auto s = statevector_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    statevector_simulator::shutdown();
    return testing::ok();
}

const int _reg_statevector_simulator = testing::register_test(
    "statevector_simulator",
    "333_statevector_simulator: stub status check",
    &run_statevector_simulator);

}
