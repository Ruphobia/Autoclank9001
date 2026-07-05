// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2077_hrv_analyzer/hrv_analyzer.hpp"

namespace {

testing::TestOutcome run_hrv_analyzer() {
    hrv_analyzer::init();
    auto s = hrv_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hrv_analyzer::shutdown();
    return testing::ok();
}

const int _reg_hrv_analyzer = testing::register_test(
    "hrv_analyzer",
    "2077_hrv_analyzer: stub status check",
    &run_hrv_analyzer);

}
