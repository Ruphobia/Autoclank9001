// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/436_ticket_tracker/ticket_tracker.hpp"

namespace {

testing::TestOutcome run_ticket_tracker() {
    ticket_tracker::init();
    auto s = ticket_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ticket_tracker::shutdown();
    return testing::ok();
}

const int _reg_ticket_tracker = testing::register_test(
    "ticket_tracker",
    "436_ticket_tracker: stub status check",
    &run_ticket_tracker);

}
