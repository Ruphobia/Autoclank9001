// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/465_root_cause_analyzer/root_cause_analyzer.hpp"

namespace {

testing::TestOutcome run_root_cause_analyzer() {
    root_cause_analyzer::init();
    auto s = root_cause_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    root_cause_analyzer::shutdown();
    return testing::ok();
}

const int _reg_root_cause_analyzer = testing::register_test(
    "root_cause_analyzer",
    "465_root_cause_analyzer: stub status check",
    &run_root_cause_analyzer);

}
