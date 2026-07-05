// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1372_restriction_and_cloning_planner/restriction_and_cloning_planner.hpp"

namespace {

testing::TestOutcome run_restriction_and_cloning_planner() {
    restriction_and_cloning_planner::init();
    auto s = restriction_and_cloning_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    restriction_and_cloning_planner::shutdown();
    return testing::ok();
}

const int _reg_restriction_and_cloning_planner = testing::register_test(
    "restriction_and_cloning_planner",
    "1372_restriction_and_cloning_planner: stub status check",
    &run_restriction_and_cloning_planner);

}
