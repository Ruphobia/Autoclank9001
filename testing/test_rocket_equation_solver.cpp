// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1134_rocket_equation_solver/rocket_equation_solver.hpp"

namespace {

testing::TestOutcome run_rocket_equation_solver() {
    rocket_equation_solver::init();
    auto s = rocket_equation_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rocket_equation_solver::shutdown();
    return testing::ok();
}

const int _reg_rocket_equation_solver = testing::register_test(
    "rocket_equation_solver",
    "1134_rocket_equation_solver: stub status check",
    &run_rocket_equation_solver);

}
