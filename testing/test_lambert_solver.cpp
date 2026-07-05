// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1131_lambert_solver/lambert_solver.hpp"

namespace {

testing::TestOutcome run_lambert_solver() {
    lambert_solver::init();
    auto s = lambert_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lambert_solver::shutdown();
    return testing::ok();
}

const int _reg_lambert_solver = testing::register_test(
    "lambert_solver",
    "1131_lambert_solver: stub status check",
    &run_lambert_solver);

}
