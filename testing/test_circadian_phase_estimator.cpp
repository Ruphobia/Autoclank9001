// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2160_circadian_phase_estimator/circadian_phase_estimator.hpp"

namespace {

testing::TestOutcome run_circadian_phase_estimator() {
    circadian_phase_estimator::init();
    auto s = circadian_phase_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    circadian_phase_estimator::shutdown();
    return testing::ok();
}

const int _reg_circadian_phase_estimator = testing::register_test(
    "circadian_phase_estimator",
    "2160_circadian_phase_estimator: stub status check",
    &run_circadian_phase_estimator);

}
