// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/493_crew_scheduler/crew_scheduler.hpp"

namespace {

testing::TestOutcome run_crew_scheduler() {
    crew_scheduler::init();
    auto s = crew_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crew_scheduler::shutdown();
    return testing::ok();
}

const int _reg_crew_scheduler = testing::register_test(
    "crew_scheduler",
    "493_crew_scheduler: stub status check",
    &run_crew_scheduler);

}
