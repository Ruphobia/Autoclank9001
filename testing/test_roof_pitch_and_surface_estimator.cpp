// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2039_roof_pitch_and_surface_estimator/roof_pitch_and_surface_estimator.hpp"

namespace {

testing::TestOutcome run_roof_pitch_and_surface_estimator() {
    roof_pitch_and_surface_estimator::init();
    auto s = roof_pitch_and_surface_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    roof_pitch_and_surface_estimator::shutdown();
    return testing::ok();
}

const int _reg_roof_pitch_and_surface_estimator = testing::register_test(
    "roof_pitch_and_surface_estimator",
    "2039_roof_pitch_and_surface_estimator: stub status check",
    &run_roof_pitch_and_surface_estimator);

}
