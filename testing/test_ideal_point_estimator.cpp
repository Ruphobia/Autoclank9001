// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1447_ideal_point_estimator/ideal_point_estimator.hpp"

namespace {

testing::TestOutcome run_ideal_point_estimator() {
    ideal_point_estimator::init();
    auto s = ideal_point_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ideal_point_estimator::shutdown();
    return testing::ok();
}

const int _reg_ideal_point_estimator = testing::register_test(
    "ideal_point_estimator",
    "1447_ideal_point_estimator: stub status check",
    &run_ideal_point_estimator);

}
