// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/485_job_shop_scheduler/job_shop_scheduler.hpp"

namespace {

testing::TestOutcome run_job_shop_scheduler() {
    job_shop_scheduler::init();
    auto s = job_shop_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    job_shop_scheduler::shutdown();
    return testing::ok();
}

const int _reg_job_shop_scheduler = testing::register_test(
    "job_shop_scheduler",
    "485_job_shop_scheduler: stub status check",
    &run_job_shop_scheduler);

}
