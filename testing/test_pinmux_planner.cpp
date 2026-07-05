// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/178_pinmux_planner/pinmux_planner.hpp"

namespace {

testing::TestOutcome run_pinmux_planner() {
    pinmux_planner::init();
    auto s = pinmux_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pinmux_planner::shutdown();
    return testing::ok();
}

const int _reg_pinmux_planner = testing::register_test(
    "pinmux_planner",
    "178_pinmux_planner: stub status check",
    &run_pinmux_planner);

}
