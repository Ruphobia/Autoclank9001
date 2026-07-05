// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1385_construct_assembly_planner/construct_assembly_planner.hpp"

namespace {

testing::TestOutcome run_construct_assembly_planner() {
    construct_assembly_planner::init();
    auto s = construct_assembly_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    construct_assembly_planner::shutdown();
    return testing::ok();
}

const int _reg_construct_assembly_planner = testing::register_test(
    "construct_assembly_planner",
    "1385_construct_assembly_planner: stub status check",
    &run_construct_assembly_planner);

}
