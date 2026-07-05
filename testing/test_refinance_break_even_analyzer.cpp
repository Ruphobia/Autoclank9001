// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/538_refinance_break_even_analyzer/refinance_break_even_analyzer.hpp"

namespace {

testing::TestOutcome run_refinance_break_even_analyzer() {
    refinance_break_even_analyzer::init();
    auto s = refinance_break_even_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    refinance_break_even_analyzer::shutdown();
    return testing::ok();
}

const int _reg_refinance_break_even_analyzer = testing::register_test(
    "refinance_break_even_analyzer",
    "538_refinance_break_even_analyzer: stub status check",
    &run_refinance_break_even_analyzer);

}
