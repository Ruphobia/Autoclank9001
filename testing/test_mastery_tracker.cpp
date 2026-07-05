#include "test_runner.hpp"
#include "../modules/525_mastery_tracker/mastery_tracker.hpp"

namespace {

testing::TestOutcome run_mastery_tracker() {
    mastery_tracker::init();
    auto s = mastery_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mastery_tracker::shutdown();
    return testing::ok();
}

const int _reg_mastery_tracker = testing::register_test(
    "mastery_tracker",
    "525_mastery_tracker: stub status check",
    &run_mastery_tracker);

}
