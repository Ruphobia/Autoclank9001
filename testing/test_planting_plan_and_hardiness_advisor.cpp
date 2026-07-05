// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2033_planting_plan_and_hardiness_advisor/planting_plan_and_hardiness_advisor.hpp"

namespace {

testing::TestOutcome run_planting_plan_and_hardiness_advisor() {
    planting_plan_and_hardiness_advisor::init();
    auto s = planting_plan_and_hardiness_advisor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    planting_plan_and_hardiness_advisor::shutdown();
    return testing::ok();
}

const int _reg_planting_plan_and_hardiness_advisor = testing::register_test(
    "planting_plan_and_hardiness_advisor",
    "2033_planting_plan_and_hardiness_advisor: stub status check",
    &run_planting_plan_and_hardiness_advisor);

}
