#include "test_runner.hpp"
#include "../modules/2182_sleep_restriction_scheduler/sleep_restriction_scheduler.hpp"

namespace {

testing::TestOutcome run_sleep_restriction_scheduler() {
    sleep_restriction_scheduler::init();
    auto s = sleep_restriction_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_restriction_scheduler::shutdown();
    return testing::ok();
}

const int _reg_sleep_restriction_scheduler = testing::register_test(
    "sleep_restriction_scheduler",
    "2182_sleep_restriction_scheduler: stub status check",
    &run_sleep_restriction_scheduler);

}
