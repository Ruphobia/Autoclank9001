// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1924_rehearsal_scheduler/rehearsal_scheduler.hpp"

namespace {

testing::TestOutcome run_rehearsal_scheduler() {
    rehearsal_scheduler::init();
    auto s = rehearsal_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rehearsal_scheduler::shutdown();
    return testing::ok();
}

const int _reg_rehearsal_scheduler = testing::register_test(
    "rehearsal_scheduler",
    "1924_rehearsal_scheduler: stub status check",
    &run_rehearsal_scheduler);

}
