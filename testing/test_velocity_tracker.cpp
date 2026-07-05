#include "test_runner.hpp"
#include "../modules/441_velocity_tracker/velocity_tracker.hpp"

namespace {

testing::TestOutcome run_velocity_tracker() {
    velocity_tracker::init();
    auto s = velocity_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    velocity_tracker::shutdown();
    return testing::ok();
}

const int _reg_velocity_tracker = testing::register_test(
    "velocity_tracker",
    "441_velocity_tracker: stub status check",
    &run_velocity_tracker);

}
