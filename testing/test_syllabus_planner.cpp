// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/529_syllabus_planner/syllabus_planner.hpp"

namespace {

testing::TestOutcome run_syllabus_planner() {
    syllabus_planner::init();
    auto s = syllabus_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    syllabus_planner::shutdown();
    return testing::ok();
}

const int _reg_syllabus_planner = testing::register_test(
    "syllabus_planner",
    "529_syllabus_planner: stub status check",
    &run_syllabus_planner);

}
