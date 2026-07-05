// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1291_lcoe_payback_analyzer/lcoe_payback_analyzer.hpp"

namespace {

testing::TestOutcome run_lcoe_payback_analyzer() {
    lcoe_payback_analyzer::init();
    auto s = lcoe_payback_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lcoe_payback_analyzer::shutdown();
    return testing::ok();
}

const int _reg_lcoe_payback_analyzer = testing::register_test(
    "lcoe_payback_analyzer",
    "1291_lcoe_payback_analyzer: stub status check",
    &run_lcoe_payback_analyzer);

}
