// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1400_dosage_calculator/dosage_calculator.hpp"

namespace {

testing::TestOutcome run_dosage_calculator() {
    dosage_calculator::init();
    auto s = dosage_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dosage_calculator::shutdown();
    return testing::ok();
}

const int _reg_dosage_calculator = testing::register_test(
    "dosage_calculator",
    "1400_dosage_calculator: stub status check",
    &run_dosage_calculator);

}
