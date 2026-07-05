// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/861_rf_design_suite/rf_design_suite.hpp"

namespace {

testing::TestOutcome run_rf_design_suite() {
    rf_design_suite::init();
    auto s = rf_design_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rf_design_suite::shutdown();
    return testing::ok();
}

const int _reg_rf_design_suite = testing::register_test(
    "rf_design_suite",
    "861_rf_design_suite: stub status check",
    &run_rf_design_suite);

}
