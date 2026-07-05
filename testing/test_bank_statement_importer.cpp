// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/556_bank_statement_importer/bank_statement_importer.hpp"

namespace {

testing::TestOutcome run_bank_statement_importer() {
    bank_statement_importer::init();
    auto s = bank_statement_importer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bank_statement_importer::shutdown();
    return testing::ok();
}

const int _reg_bank_statement_importer = testing::register_test(
    "bank_statement_importer",
    "556_bank_statement_importer: stub status check",
    &run_bank_statement_importer);

}
