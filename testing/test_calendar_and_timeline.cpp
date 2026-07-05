#include "test_runner.hpp"
#include "../modules/1959_calendar_and_timeline/calendar_and_timeline.hpp"

namespace {

testing::TestOutcome run_calendar_and_timeline() {
    calendar_and_timeline::init();
    auto s = calendar_and_timeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    calendar_and_timeline::shutdown();
    return testing::ok();
}

const int _reg_calendar_and_timeline = testing::register_test(
    "calendar_and_timeline",
    "1959_calendar_and_timeline: stub status check",
    &run_calendar_and_timeline);

}
