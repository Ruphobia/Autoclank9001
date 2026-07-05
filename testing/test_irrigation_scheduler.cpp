#include "test_runner.hpp"
#include "../modules/1603_irrigation_scheduler/irrigation_scheduler.hpp"

namespace {

testing::TestOutcome run_irrigation_scheduler() {
    irrigation_scheduler::init();
    auto s = irrigation_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    irrigation_scheduler::shutdown();
    return testing::ok();
}

const int _reg_irrigation_scheduler = testing::register_test(
    "irrigation_scheduler",
    "1603_irrigation_scheduler: stub status check",
    &run_irrigation_scheduler);

}
