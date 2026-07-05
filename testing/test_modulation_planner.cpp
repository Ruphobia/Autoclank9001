// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1720_modulation_planner/modulation_planner.hpp"

namespace {

testing::TestOutcome run_modulation_planner() {
    modulation_planner::init();
    auto s = modulation_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    modulation_planner::shutdown();
    return testing::ok();
}

const int _reg_modulation_planner = testing::register_test(
    "modulation_planner",
    "1720_modulation_planner: stub status check",
    &run_modulation_planner);

}
