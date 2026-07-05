// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1575_conservation_prioritization_solver/conservation_prioritization_solver.hpp"

namespace {

testing::TestOutcome run_conservation_prioritization_solver() {
    conservation_prioritization_solver::init();
    auto s = conservation_prioritization_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    conservation_prioritization_solver::shutdown();
    return testing::ok();
}

const int _reg_conservation_prioritization_solver = testing::register_test(
    "conservation_prioritization_solver",
    "1575_conservation_prioritization_solver: stub status check",
    &run_conservation_prioritization_solver);

}
