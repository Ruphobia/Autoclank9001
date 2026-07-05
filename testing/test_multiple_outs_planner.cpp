// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1940_multiple_outs_planner/multiple_outs_planner.hpp"

namespace {

testing::TestOutcome run_multiple_outs_planner() {
    multiple_outs_planner::init();
    auto s = multiple_outs_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multiple_outs_planner::shutdown();
    return testing::ok();
}

const int _reg_multiple_outs_planner = testing::register_test(
    "multiple_outs_planner",
    "1940_multiple_outs_planner: stub status check",
    &run_multiple_outs_planner);

}
