// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2162_sleep_debt_ledger/sleep_debt_ledger.hpp"

namespace {

testing::TestOutcome run_sleep_debt_ledger() {
    sleep_debt_ledger::init();
    auto s = sleep_debt_ledger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_debt_ledger::shutdown();
    return testing::ok();
}

const int _reg_sleep_debt_ledger = testing::register_test(
    "sleep_debt_ledger",
    "2162_sleep_debt_ledger: stub status check",
    &run_sleep_debt_ledger);

}
