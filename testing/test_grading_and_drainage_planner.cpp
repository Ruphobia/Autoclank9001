// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2034_grading_and_drainage_planner/grading_and_drainage_planner.hpp"

namespace {

testing::TestOutcome run_grading_and_drainage_planner() {
    grading_and_drainage_planner::init();
    auto s = grading_and_drainage_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grading_and_drainage_planner::shutdown();
    return testing::ok();
}

const int _reg_grading_and_drainage_planner = testing::register_test(
    "grading_and_drainage_planner",
    "2034_grading_and_drainage_planner: stub status check",
    &run_grading_and_drainage_planner);

}
