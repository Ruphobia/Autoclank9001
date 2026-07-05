#include "test_runner.hpp"
#include "../modules/2072_periodization_scheduler/periodization_scheduler.hpp"

namespace {

testing::TestOutcome run_periodization_scheduler() {
    periodization_scheduler::init();
    auto s = periodization_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    periodization_scheduler::shutdown();
    return testing::ok();
}

const int _reg_periodization_scheduler = testing::register_test(
    "periodization_scheduler",
    "2072_periodization_scheduler: stub status check",
    &run_periodization_scheduler);

}
