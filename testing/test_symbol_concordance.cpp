// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/781_symbol_concordance/symbol_concordance.hpp"

namespace {

testing::TestOutcome run_symbol_concordance() {
    symbol_concordance::init();
    auto s = symbol_concordance::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    symbol_concordance::shutdown();
    return testing::ok();
}

const int _reg_symbol_concordance = testing::register_test(
    "symbol_concordance",
    "781_symbol_concordance: stub status check",
    &run_symbol_concordance);

}
