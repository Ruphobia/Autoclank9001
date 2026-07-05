// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/907_em_field_solver/em_field_solver.hpp"

namespace {

testing::TestOutcome run_em_field_solver() {
    em_field_solver::init();
    auto s = em_field_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    em_field_solver::shutdown();
    return testing::ok();
}

const int _reg_em_field_solver = testing::register_test(
    "em_field_solver",
    "907_em_field_solver: stub status check",
    &run_em_field_solver);

}
