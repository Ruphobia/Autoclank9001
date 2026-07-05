// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1112_fatigue_and_s_n_analyzer/fatigue_and_s_n_analyzer.hpp"

namespace {

testing::TestOutcome run_fatigue_and_s_n_analyzer() {
    fatigue_and_s_n_analyzer::init();
    auto s = fatigue_and_s_n_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fatigue_and_s_n_analyzer::shutdown();
    return testing::ok();
}

const int _reg_fatigue_and_s_n_analyzer = testing::register_test(
    "fatigue_and_s_n_analyzer",
    "1112_fatigue_and_s_n_analyzer: stub status check",
    &run_fatigue_and_s_n_analyzer);

}
