// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1889_bullion_and_melt_calculator/bullion_and_melt_calculator.hpp"

namespace {

testing::TestOutcome run_bullion_and_melt_calculator() {
    bullion_and_melt_calculator::init();
    auto s = bullion_and_melt_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bullion_and_melt_calculator::shutdown();
    return testing::ok();
}

const int _reg_bullion_and_melt_calculator = testing::register_test(
    "bullion_and_melt_calculator",
    "1889_bullion_and_melt_calculator: stub status check",
    &run_bullion_and_melt_calculator);

}
