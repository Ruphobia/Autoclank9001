// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1843_heat_treatment_planner/heat_treatment_planner.hpp"

namespace {

testing::TestOutcome run_heat_treatment_planner() {
    heat_treatment_planner::init();
    auto s = heat_treatment_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    heat_treatment_planner::shutdown();
    return testing::ok();
}

const int _reg_heat_treatment_planner = testing::register_test(
    "heat_treatment_planner",
    "1843_heat_treatment_planner: stub status check",
    &run_heat_treatment_planner);

}
