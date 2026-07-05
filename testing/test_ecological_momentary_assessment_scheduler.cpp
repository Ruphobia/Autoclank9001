// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1433_ecological_momentary_assessment_scheduler/ecological_momentary_assessment_scheduler.hpp"

namespace {

testing::TestOutcome run_ecological_momentary_assessment_scheduler() {
    ecological_momentary_assessment_scheduler::init();
    auto s = ecological_momentary_assessment_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ecological_momentary_assessment_scheduler::shutdown();
    return testing::ok();
}

const int _reg_ecological_momentary_assessment_scheduler = testing::register_test(
    "ecological_momentary_assessment_scheduler",
    "1433_ecological_momentary_assessment_scheduler: stub status check",
    &run_ecological_momentary_assessment_scheduler);

}
