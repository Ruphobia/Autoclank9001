// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/347_resource_estimator/resource_estimator.hpp"

namespace {

testing::TestOutcome run_resource_estimator() {
    resource_estimator::init();
    auto s = resource_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    resource_estimator::shutdown();
    return testing::ok();
}

const int _reg_resource_estimator = testing::register_test(
    "resource_estimator",
    "347_resource_estimator: stub status check",
    &run_resource_estimator);

}
