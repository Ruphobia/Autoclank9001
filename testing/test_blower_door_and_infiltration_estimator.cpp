// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2029_blower_door_and_infiltration_estimator/blower_door_and_infiltration_estimator.hpp"

namespace {

testing::TestOutcome run_blower_door_and_infiltration_estimator() {
    blower_door_and_infiltration_estimator::init();
    auto s = blower_door_and_infiltration_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    blower_door_and_infiltration_estimator::shutdown();
    return testing::ok();
}

const int _reg_blower_door_and_infiltration_estimator = testing::register_test(
    "blower_door_and_infiltration_estimator",
    "2029_blower_door_and_infiltration_estimator: stub status check",
    &run_blower_door_and_infiltration_estimator);

}
