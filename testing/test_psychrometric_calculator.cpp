// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1017_psychrometric_calculator/psychrometric_calculator.hpp"

namespace {

testing::TestOutcome run_psychrometric_calculator() {
    psychrometric_calculator::init();
    auto s = psychrometric_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    psychrometric_calculator::shutdown();
    return testing::ok();
}

const int _reg_psychrometric_calculator = testing::register_test(
    "psychrometric_calculator",
    "1017_psychrometric_calculator: stub status check",
    &run_psychrometric_calculator);

}
