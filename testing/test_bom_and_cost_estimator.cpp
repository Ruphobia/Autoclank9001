// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/951_bom_and_cost_estimator/bom_and_cost_estimator.hpp"

namespace {

testing::TestOutcome run_bom_and_cost_estimator() {
    bom_and_cost_estimator::init();
    auto s = bom_and_cost_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bom_and_cost_estimator::shutdown();
    return testing::ok();
}

const int _reg_bom_and_cost_estimator = testing::register_test(
    "bom_and_cost_estimator",
    "951_bom_and_cost_estimator: stub status check",
    &run_bom_and_cost_estimator);

}
