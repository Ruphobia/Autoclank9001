#include "test_runner.hpp"
#include "../modules/293_tracker_studio/tracker_studio.hpp"

namespace {

testing::TestOutcome run_tracker_studio() {
    tracker_studio::init();
    auto s = tracker_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tracker_studio::shutdown();
    return testing::ok();
}

const int _reg_tracker_studio = testing::register_test(
    "tracker_studio",
    "293_tracker_studio: stub status check",
    &run_tracker_studio);

}
