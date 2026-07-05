// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1535_body_size_and_mass_estimator/body_size_and_mass_estimator.hpp"

namespace {

testing::TestOutcome run_body_size_and_mass_estimator() {
    body_size_and_mass_estimator::init();
    auto s = body_size_and_mass_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    body_size_and_mass_estimator::shutdown();
    return testing::ok();
}

const int _reg_body_size_and_mass_estimator = testing::register_test(
    "body_size_and_mass_estimator",
    "1535_body_size_and_mass_estimator: stub status check",
    &run_body_size_and_mass_estimator);

}
