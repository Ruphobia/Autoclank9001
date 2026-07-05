// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1001_tribology_and_wear_estimator/tribology_and_wear_estimator.hpp"

namespace {

testing::TestOutcome run_tribology_and_wear_estimator() {
    tribology_and_wear_estimator::init();
    auto s = tribology_and_wear_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tribology_and_wear_estimator::shutdown();
    return testing::ok();
}

const int _reg_tribology_and_wear_estimator = testing::register_test(
    "tribology_and_wear_estimator",
    "1001_tribology_and_wear_estimator: stub status check",
    &run_tribology_and_wear_estimator);

}
