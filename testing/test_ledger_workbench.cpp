// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/553_ledger_workbench/ledger_workbench.hpp"

namespace {

testing::TestOutcome run_ledger_workbench() {
    ledger_workbench::init();
    auto s = ledger_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ledger_workbench::shutdown();
    return testing::ok();
}

const int _reg_ledger_workbench = testing::register_test(
    "ledger_workbench",
    "553_ledger_workbench: stub status check",
    &run_ledger_workbench);

}
