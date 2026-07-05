// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1401_pharmacokinetics_simulator/pharmacokinetics_simulator.hpp"

namespace {

testing::TestOutcome run_pharmacokinetics_simulator() {
    pharmacokinetics_simulator::init();
    auto s = pharmacokinetics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pharmacokinetics_simulator::shutdown();
    return testing::ok();
}

const int _reg_pharmacokinetics_simulator = testing::register_test(
    "pharmacokinetics_simulator",
    "1401_pharmacokinetics_simulator: stub status check",
    &run_pharmacokinetics_simulator);

}
