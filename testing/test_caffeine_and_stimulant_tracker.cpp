#include "test_runner.hpp"
#include "../modules/2166_caffeine_and_stimulant_tracker/caffeine_and_stimulant_tracker.hpp"

namespace {

testing::TestOutcome run_caffeine_and_stimulant_tracker() {
    caffeine_and_stimulant_tracker::init();
    auto s = caffeine_and_stimulant_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    caffeine_and_stimulant_tracker::shutdown();
    return testing::ok();
}

const int _reg_caffeine_and_stimulant_tracker = testing::register_test(
    "caffeine_and_stimulant_tracker",
    "2166_caffeine_and_stimulant_tracker: stub status check",
    &run_caffeine_and_stimulant_tracker);

}
