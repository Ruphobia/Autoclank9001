// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1087_material_balance_solver/material_balance_solver.hpp"

namespace {

testing::TestOutcome run_material_balance_solver() {
    material_balance_solver::init();
    auto s = material_balance_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    material_balance_solver::shutdown();
    return testing::ok();
}

const int _reg_material_balance_solver = testing::register_test(
    "material_balance_solver",
    "1087_material_balance_solver: stub status check",
    &run_material_balance_solver);

}
