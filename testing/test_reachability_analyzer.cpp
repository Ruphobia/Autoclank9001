// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1250_reachability_analyzer/reachability_analyzer.hpp"

namespace {

testing::TestOutcome run_reachability_analyzer() {
    reachability_analyzer::init();
    auto s = reachability_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reachability_analyzer::shutdown();
    return testing::ok();
}

const int _reg_reachability_analyzer = testing::register_test(
    "reachability_analyzer",
    "1250_reachability_analyzer: stub status check",
    &run_reachability_analyzer);

}
