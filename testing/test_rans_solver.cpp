// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1159_rans_solver/rans_solver.hpp"

namespace {

testing::TestOutcome run_rans_solver() {
    rans_solver::init();
    auto s = rans_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rans_solver::shutdown();
    return testing::ok();
}

const int _reg_rans_solver = testing::register_test(
    "rans_solver",
    "1159_rans_solver: stub status check",
    &run_rans_solver);

}
