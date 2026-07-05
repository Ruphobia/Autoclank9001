// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1157_wing_and_fuselage_drag_estimator/wing_and_fuselage_drag_estimator.hpp"

namespace {

testing::TestOutcome run_wing_and_fuselage_drag_estimator() {
    wing_and_fuselage_drag_estimator::init();
    auto s = wing_and_fuselage_drag_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wing_and_fuselage_drag_estimator::shutdown();
    return testing::ok();
}

const int _reg_wing_and_fuselage_drag_estimator = testing::register_test(
    "wing_and_fuselage_drag_estimator",
    "1157_wing_and_fuselage_drag_estimator: stub status check",
    &run_wing_and_fuselage_drag_estimator);

}
