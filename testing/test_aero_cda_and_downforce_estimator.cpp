// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1215_aero_cda_and_downforce_estimator/aero_cda_and_downforce_estimator.hpp"

namespace {

testing::TestOutcome run_aero_cda_and_downforce_estimator() {
    aero_cda_and_downforce_estimator::init();
    auto s = aero_cda_and_downforce_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    aero_cda_and_downforce_estimator::shutdown();
    return testing::ok();
}

const int _reg_aero_cda_and_downforce_estimator = testing::register_test(
    "aero_cda_and_downforce_estimator",
    "1215_aero_cda_and_downforce_estimator: stub status check",
    &run_aero_cda_and_downforce_estimator);

}
