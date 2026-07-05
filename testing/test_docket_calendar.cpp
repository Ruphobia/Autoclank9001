// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/592_docket_calendar/docket_calendar.hpp"

namespace {

testing::TestOutcome run_docket_calendar() {
    docket_calendar::init();
    auto s = docket_calendar::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    docket_calendar::shutdown();
    return testing::ok();
}

const int _reg_docket_calendar = testing::register_test(
    "docket_calendar",
    "592_docket_calendar: stub status check",
    &run_docket_calendar);

}
