// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/991_jominy_hardenability_estimator/jominy_hardenability_estimator.hpp"

namespace {

testing::TestOutcome run_jominy_hardenability_estimator() {
    jominy_hardenability_estimator::init();
    auto s = jominy_hardenability_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    jominy_hardenability_estimator::shutdown();
    return testing::ok();
}

const int _reg_jominy_hardenability_estimator = testing::register_test(
    "jominy_hardenability_estimator",
    "991_jominy_hardenability_estimator: stub status check",
    &run_jominy_hardenability_estimator);

}
