// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/486_bullwhip_simulator/bullwhip_simulator.hpp"

namespace {

testing::TestOutcome run_bullwhip_simulator() {
    bullwhip_simulator::init();
    auto s = bullwhip_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bullwhip_simulator::shutdown();
    return testing::ok();
}

const int _reg_bullwhip_simulator = testing::register_test(
    "bullwhip_simulator",
    "486_bullwhip_simulator: stub status check",
    &run_bullwhip_simulator);

}
