// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/910_heat_transfer_solver/heat_transfer_solver.hpp"

namespace {

testing::TestOutcome run_heat_transfer_solver() {
    heat_transfer_solver::init();
    auto s = heat_transfer_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    heat_transfer_solver::shutdown();
    return testing::ok();
}

const int _reg_heat_transfer_solver = testing::register_test(
    "heat_transfer_solver",
    "910_heat_transfer_solver: stub status check",
    &run_heat_transfer_solver);

}
