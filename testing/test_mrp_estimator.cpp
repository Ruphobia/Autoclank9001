// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1444_mrp_estimator/mrp_estimator.hpp"

namespace {

testing::TestOutcome run_mrp_estimator() {
    mrp_estimator::init();
    auto s = mrp_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mrp_estimator::shutdown();
    return testing::ok();
}

const int _reg_mrp_estimator = testing::register_test(
    "mrp_estimator",
    "1444_mrp_estimator: stub status check",
    &run_mrp_estimator);

}
