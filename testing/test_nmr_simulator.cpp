// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1302_nmr_simulator/nmr_simulator.hpp"

namespace {

testing::TestOutcome run_nmr_simulator() {
    nmr_simulator::init();
    auto s = nmr_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nmr_simulator::shutdown();
    return testing::ok();
}

const int _reg_nmr_simulator = testing::register_test(
    "nmr_simulator",
    "1302_nmr_simulator: stub status check",
    &run_nmr_simulator);

}
