// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1869_calligraphy_nib_simulator/calligraphy_nib_simulator.hpp"

namespace {

testing::TestOutcome run_calligraphy_nib_simulator() {
    calligraphy_nib_simulator::init();
    auto s = calligraphy_nib_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    calligraphy_nib_simulator::shutdown();
    return testing::ok();
}

const int _reg_calligraphy_nib_simulator = testing::register_test(
    "calligraphy_nib_simulator",
    "1869_calligraphy_nib_simulator: stub status check",
    &run_calligraphy_nib_simulator);

}
