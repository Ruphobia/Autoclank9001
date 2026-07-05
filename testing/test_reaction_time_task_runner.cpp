// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1426_reaction_time_task_runner/reaction_time_task_runner.hpp"

namespace {

testing::TestOutcome run_reaction_time_task_runner() {
    reaction_time_task_runner::init();
    auto s = reaction_time_task_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reaction_time_task_runner::shutdown();
    return testing::ok();
}

const int _reg_reaction_time_task_runner = testing::register_test(
    "reaction_time_task_runner",
    "1426_reaction_time_task_runner: stub status check",
    &run_reaction_time_task_runner);

}
