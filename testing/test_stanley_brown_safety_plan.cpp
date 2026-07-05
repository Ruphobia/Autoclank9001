// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2180_stanley_brown_safety_plan/stanley_brown_safety_plan.hpp"

namespace {

testing::TestOutcome run_stanley_brown_safety_plan() {
    stanley_brown_safety_plan::init();
    auto s = stanley_brown_safety_plan::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stanley_brown_safety_plan::shutdown();
    return testing::ok();
}

const int _reg_stanley_brown_safety_plan = testing::register_test(
    "stanley_brown_safety_plan",
    "2180_stanley_brown_safety_plan: stub status check",
    &run_stanley_brown_safety_plan);

}
