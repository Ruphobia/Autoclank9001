// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/478_vrp_solver/vrp_solver.hpp"

namespace {

testing::TestOutcome run_vrp_solver() {
    vrp_solver::init();
    auto s = vrp_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vrp_solver::shutdown();
    return testing::ok();
}

const int _reg_vrp_solver = testing::register_test(
    "vrp_solver",
    "478_vrp_solver: stub status check",
    &run_vrp_solver);

}
