// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1081_well_log_analyzer/well_log_analyzer.hpp"

namespace {

testing::TestOutcome run_well_log_analyzer() {
    well_log_analyzer::init();
    auto s = well_log_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    well_log_analyzer::shutdown();
    return testing::ok();
}

const int _reg_well_log_analyzer = testing::register_test(
    "well_log_analyzer",
    "1081_well_log_analyzer: stub status check",
    &run_well_log_analyzer);

}
