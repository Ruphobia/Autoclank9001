// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1056_quantity_takeoff_cost_estimator/quantity_takeoff_cost_estimator.hpp"

namespace {

testing::TestOutcome run_quantity_takeoff_cost_estimator() {
    quantity_takeoff_cost_estimator::init();
    auto s = quantity_takeoff_cost_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quantity_takeoff_cost_estimator::shutdown();
    return testing::ok();
}

const int _reg_quantity_takeoff_cost_estimator = testing::register_test(
    "quantity_takeoff_cost_estimator",
    "1056_quantity_takeoff_cost_estimator: stub status check",
    &run_quantity_takeoff_cost_estimator);

}
