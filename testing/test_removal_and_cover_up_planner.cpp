// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1910_removal_and_cover_up_planner/removal_and_cover_up_planner.hpp"

namespace {

testing::TestOutcome run_removal_and_cover_up_planner() {
    removal_and_cover_up_planner::init();
    auto s = removal_and_cover_up_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    removal_and_cover_up_planner::shutdown();
    return testing::ok();
}

const int _reg_removal_and_cover_up_planner = testing::register_test(
    "removal_and_cover_up_planner",
    "1910_removal_and_cover_up_planner: stub status check",
    &run_removal_and_cover_up_planner);

}
