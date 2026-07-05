// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1666_exposure_calculator/exposure_calculator.hpp"

namespace {

testing::TestOutcome run_exposure_calculator() {
    exposure_calculator::init();
    auto s = exposure_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    exposure_calculator::shutdown();
    return testing::ok();
}

const int _reg_exposure_calculator = testing::register_test(
    "exposure_calculator",
    "1666_exposure_calculator: stub status check",
    &run_exposure_calculator);

}
