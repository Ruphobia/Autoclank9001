// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1916_photometric_calculator/photometric_calculator.hpp"

namespace {

testing::TestOutcome run_photometric_calculator() {
    photometric_calculator::init();
    auto s = photometric_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photometric_calculator::shutdown();
    return testing::ok();
}

const int _reg_photometric_calculator = testing::register_test(
    "photometric_calculator",
    "1916_photometric_calculator: stub status check",
    &run_photometric_calculator);

}
