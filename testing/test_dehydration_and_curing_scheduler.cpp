#include "test_runner.hpp"
#include "../modules/1808_dehydration_and_curing_scheduler/dehydration_and_curing_scheduler.hpp"

namespace {

testing::TestOutcome run_dehydration_and_curing_scheduler() {
    dehydration_and_curing_scheduler::init();
    auto s = dehydration_and_curing_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dehydration_and_curing_scheduler::shutdown();
    return testing::ok();
}

const int _reg_dehydration_and_curing_scheduler = testing::register_test(
    "dehydration_and_curing_scheduler",
    "1808_dehydration_and_curing_scheduler: stub status check",
    &run_dehydration_and_curing_scheduler);

}
