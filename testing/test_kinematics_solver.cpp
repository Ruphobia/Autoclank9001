// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1247_kinematics_solver/kinematics_solver.hpp"

namespace {

testing::TestOutcome run_kinematics_solver() {
    kinematics_solver::init();
    auto s = kinematics_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    kinematics_solver::shutdown();
    return testing::ok();
}

const int _reg_kinematics_solver = testing::register_test(
    "kinematics_solver",
    "1247_kinematics_solver: stub status check",
    &run_kinematics_solver);

}
