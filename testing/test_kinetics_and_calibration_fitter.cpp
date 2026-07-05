// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1316_kinetics_and_calibration_fitter/kinetics_and_calibration_fitter.hpp"

namespace {

testing::TestOutcome run_kinetics_and_calibration_fitter() {
    kinetics_and_calibration_fitter::init();
    auto s = kinetics_and_calibration_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    kinetics_and_calibration_fitter::shutdown();
    return testing::ok();
}

const int _reg_kinetics_and_calibration_fitter = testing::register_test(
    "kinetics_and_calibration_fitter",
    "1316_kinetics_and_calibration_fitter: stub status check",
    &run_kinetics_and_calibration_fitter);

}
