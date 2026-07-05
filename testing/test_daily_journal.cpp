// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/418_daily_journal/daily_journal.hpp"

namespace {

testing::TestOutcome run_daily_journal() {
    daily_journal::init();
    auto s = daily_journal::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    daily_journal::shutdown();
    return testing::ok();
}

const int _reg_daily_journal = testing::register_test(
    "daily_journal",
    "418_daily_journal: stub status check",
    &run_daily_journal);

}
