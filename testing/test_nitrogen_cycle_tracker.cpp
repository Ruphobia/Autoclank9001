#include "test_runner.hpp"
#include "../modules/2008_nitrogen_cycle_tracker/nitrogen_cycle_tracker.hpp"

namespace {

testing::TestOutcome run_nitrogen_cycle_tracker() {
    nitrogen_cycle_tracker::init();
    auto s = nitrogen_cycle_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nitrogen_cycle_tracker::shutdown();
    return testing::ok();
}

const int _reg_nitrogen_cycle_tracker = testing::register_test(
    "nitrogen_cycle_tracker",
    "2008_nitrogen_cycle_tracker: stub status check",
    &run_nitrogen_cycle_tracker);

}
