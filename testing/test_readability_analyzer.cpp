// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/501_readability_analyzer/readability_analyzer.hpp"

namespace {

testing::TestOutcome run_readability_analyzer() {
    readability_analyzer::init();
    auto s = readability_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    readability_analyzer::shutdown();
    return testing::ok();
}

const int _reg_readability_analyzer = testing::register_test(
    "readability_analyzer",
    "501_readability_analyzer: stub status check",
    &run_readability_analyzer);

}
