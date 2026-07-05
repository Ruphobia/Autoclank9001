#include "test_runner.hpp"
#include "../modules/1977_speedrun_split_timer/speedrun_split_timer.hpp"

namespace {

testing::TestOutcome run_speedrun_split_timer() {
    speedrun_split_timer::init();
    auto s = speedrun_split_timer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    speedrun_split_timer::shutdown();
    return testing::ok();
}

const int _reg_speedrun_split_timer = testing::register_test(
    "speedrun_split_timer",
    "1977_speedrun_split_timer: stub status check",
    &run_speedrun_split_timer);

}
