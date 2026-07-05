// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/853_power_integrity_analyzer/power_integrity_analyzer.hpp"

namespace {

testing::TestOutcome run_power_integrity_analyzer() {
    power_integrity_analyzer::init();
    auto s = power_integrity_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    power_integrity_analyzer::shutdown();
    return testing::ok();
}

const int _reg_power_integrity_analyzer = testing::register_test(
    "power_integrity_analyzer",
    "853_power_integrity_analyzer: stub status check",
    &run_power_integrity_analyzer);

}
