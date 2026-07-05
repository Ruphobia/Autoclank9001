#include "test_runner.hpp"
#include "../modules/421_calendar/calendar.hpp"

namespace {

testing::TestOutcome run_calendar() {
    calendar::init();
    auto s = calendar::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    calendar::shutdown();
    return testing::ok();
}

const int _reg_calendar = testing::register_test(
    "calendar",
    "421_calendar: stub status check",
    &run_calendar);

}
