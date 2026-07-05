#include "test_runner.hpp"
#include "../modules/2156_reality_check_scheduler/reality_check_scheduler.hpp"

namespace {

testing::TestOutcome run_reality_check_scheduler() {
    reality_check_scheduler::init();
    auto s = reality_check_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reality_check_scheduler::shutdown();
    return testing::ok();
}

const int _reg_reality_check_scheduler = testing::register_test(
    "reality_check_scheduler",
    "2156_reality_check_scheduler: stub status check",
    &run_reality_check_scheduler);

}
