// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/612_audit_trail_ledger/audit_trail_ledger.hpp"

namespace {

testing::TestOutcome run_audit_trail_ledger() {
    audit_trail_ledger::init();
    auto s = audit_trail_ledger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audit_trail_ledger::shutdown();
    return testing::ok();
}

const int _reg_audit_trail_ledger = testing::register_test(
    "audit_trail_ledger",
    "612_audit_trail_ledger: stub status check",
    &run_audit_trail_ledger);

}
