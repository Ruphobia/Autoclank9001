#include "test_runner.hpp"
#include "../modules/438_gantt_scheduler/gantt_scheduler.hpp"

namespace {

testing::TestOutcome run_gantt_scheduler() {
    gantt_scheduler::init();
    auto s = gantt_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gantt_scheduler::shutdown();
    return testing::ok();
}

const int _reg_gantt_scheduler = testing::register_test(
    "gantt_scheduler",
    "438_gantt_scheduler: stub status check",
    &run_gantt_scheduler);

}
