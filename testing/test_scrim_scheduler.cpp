#include "test_runner.hpp"
#include "../modules/1997_scrim_scheduler/scrim_scheduler.hpp"

namespace {

testing::TestOutcome run_scrim_scheduler() {
    scrim_scheduler::init();
    auto s = scrim_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scrim_scheduler::shutdown();
    return testing::ok();
}

const int _reg_scrim_scheduler = testing::register_test(
    "scrim_scheduler",
    "1997_scrim_scheduler: stub status check",
    &run_scrim_scheduler);

}
