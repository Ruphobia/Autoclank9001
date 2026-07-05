#include "test_runner.hpp"
#include "../modules/775_ritual_and_festival_calendar/ritual_and_festival_calendar.hpp"

namespace {

testing::TestOutcome run_ritual_and_festival_calendar() {
    ritual_and_festival_calendar::init();
    auto s = ritual_and_festival_calendar::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ritual_and_festival_calendar::shutdown();
    return testing::ok();
}

const int _reg_ritual_and_festival_calendar = testing::register_test(
    "ritual_and_festival_calendar",
    "775_ritual_and_festival_calendar: stub status check",
    &run_ritual_and_festival_calendar);

}
