// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/343_variational_solver_lab/variational_solver_lab.hpp"

namespace {

testing::TestOutcome run_variational_solver_lab() {
    variational_solver_lab::init();
    auto s = variational_solver_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    variational_solver_lab::shutdown();
    return testing::ok();
}

const int _reg_variational_solver_lab = testing::register_test(
    "variational_solver_lab",
    "343_variational_solver_lab: stub status check",
    &run_variational_solver_lab);

}
