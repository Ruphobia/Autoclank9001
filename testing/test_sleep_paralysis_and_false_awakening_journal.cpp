// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2170_sleep_paralysis_and_false_awakening_journal/sleep_paralysis_and_false_awakening_journal.hpp"

namespace {

testing::TestOutcome run_sleep_paralysis_and_false_awakening_journal() {
    sleep_paralysis_and_false_awakening_journal::init();
    auto s = sleep_paralysis_and_false_awakening_journal::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_paralysis_and_false_awakening_journal::shutdown();
    return testing::ok();
}

const int _reg_sleep_paralysis_and_false_awakening_journal = testing::register_test(
    "sleep_paralysis_and_false_awakening_journal",
    "2170_sleep_paralysis_and_false_awakening_journal: stub status check",
    &run_sleep_paralysis_and_false_awakening_journal);

}
