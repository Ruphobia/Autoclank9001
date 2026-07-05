// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1852_annealing_and_lehr_planner/annealing_and_lehr_planner.hpp"

namespace {

testing::TestOutcome run_annealing_and_lehr_planner() {
    annealing_and_lehr_planner::init();
    auto s = annealing_and_lehr_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    annealing_and_lehr_planner::shutdown();
    return testing::ok();
}

const int _reg_annealing_and_lehr_planner = testing::register_test(
    "annealing_and_lehr_planner",
    "1852_annealing_and_lehr_planner: stub status check",
    &run_annealing_and_lehr_planner);

}
