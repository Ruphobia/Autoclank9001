#include "test_runner.hpp"
#include "../modules/2175_meditation_timer_with_interval_bells/meditation_timer_with_interval_bells.hpp"

namespace {

testing::TestOutcome run_meditation_timer_with_interval_bells() {
    meditation_timer_with_interval_bells::init();
    auto s = meditation_timer_with_interval_bells::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meditation_timer_with_interval_bells::shutdown();
    return testing::ok();
}

const int _reg_meditation_timer_with_interval_bells = testing::register_test(
    "meditation_timer_with_interval_bells",
    "2175_meditation_timer_with_interval_bells: stub status check",
    &run_meditation_timer_with_interval_bells);

}
