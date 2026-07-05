// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1512_vaccine_and_prophylaxis_scheduler/vaccine_and_prophylaxis_scheduler.hpp"

namespace {

testing::TestOutcome run_vaccine_and_prophylaxis_scheduler() {
    vaccine_and_prophylaxis_scheduler::init();
    auto s = vaccine_and_prophylaxis_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vaccine_and_prophylaxis_scheduler::shutdown();
    return testing::ok();
}

const int _reg_vaccine_and_prophylaxis_scheduler = testing::register_test(
    "vaccine_and_prophylaxis_scheduler",
    "1512_vaccine_and_prophylaxis_scheduler: stub status check",
    &run_vaccine_and_prophylaxis_scheduler);

}
