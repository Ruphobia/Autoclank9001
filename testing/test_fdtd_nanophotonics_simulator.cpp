// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/882_fdtd_nanophotonics_simulator/fdtd_nanophotonics_simulator.hpp"

namespace {

testing::TestOutcome run_fdtd_nanophotonics_simulator() {
    fdtd_nanophotonics_simulator::init();
    auto s = fdtd_nanophotonics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fdtd_nanophotonics_simulator::shutdown();
    return testing::ok();
}

const int _reg_fdtd_nanophotonics_simulator = testing::register_test(
    "fdtd_nanophotonics_simulator",
    "882_fdtd_nanophotonics_simulator: stub status check",
    &run_fdtd_nanophotonics_simulator);

}
