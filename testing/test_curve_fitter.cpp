// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/898_curve_fitter/curve_fitter.hpp"

namespace {

testing::TestOutcome run_curve_fitter() {
    curve_fitter::init();
    auto s = curve_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    curve_fitter::shutdown();
    return testing::ok();
}

const int _reg_curve_fitter = testing::register_test(
    "curve_fitter",
    "898_curve_fitter: stub status check",
    &run_curve_fitter);

}
