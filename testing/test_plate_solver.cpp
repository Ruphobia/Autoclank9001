// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1480_plate_solver/plate_solver.hpp"

namespace {

testing::TestOutcome run_plate_solver() {
    plate_solver::init();
    auto s = plate_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plate_solver::shutdown();
    return testing::ok();
}

const int _reg_plate_solver = testing::register_test(
    "plate_solver",
    "1480_plate_solver: stub status check",
    &run_plate_solver);

}
