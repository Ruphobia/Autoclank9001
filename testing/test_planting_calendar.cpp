// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1600_planting_calendar/planting_calendar.hpp"

namespace {

testing::TestOutcome run_planting_calendar() {
    planting_calendar::init();
    auto s = planting_calendar::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    planting_calendar::shutdown();
    return testing::ok();
}

const int _reg_planting_calendar = testing::register_test(
    "planting_calendar",
    "1600_planting_calendar: stub status check",
    &run_planting_calendar);

}
