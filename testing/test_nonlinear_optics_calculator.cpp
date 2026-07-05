// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/889_nonlinear_optics_calculator/nonlinear_optics_calculator.hpp"

namespace {

testing::TestOutcome run_nonlinear_optics_calculator() {
    nonlinear_optics_calculator::init();
    auto s = nonlinear_optics_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nonlinear_optics_calculator::shutdown();
    return testing::ok();
}

const int _reg_nonlinear_optics_calculator = testing::register_test(
    "nonlinear_optics_calculator",
    "889_nonlinear_optics_calculator: stub status check",
    &run_nonlinear_optics_calculator);

}
