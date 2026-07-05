// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2169_dream_incubation_planner/dream_incubation_planner.hpp"

namespace {

testing::TestOutcome run_dream_incubation_planner() {
    dream_incubation_planner::init();
    auto s = dream_incubation_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dream_incubation_planner::shutdown();
    return testing::ok();
}

const int _reg_dream_incubation_planner = testing::register_test(
    "dream_incubation_planner",
    "2169_dream_incubation_planner: stub status check",
    &run_dream_incubation_planner);

}
