// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1837_gear_train_calculator/gear_train_calculator.hpp"

namespace {

testing::TestOutcome run_gear_train_calculator() {
    gear_train_calculator::init();
    auto s = gear_train_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gear_train_calculator::shutdown();
    return testing::ok();
}

const int _reg_gear_train_calculator = testing::register_test(
    "gear_train_calculator",
    "1837_gear_train_calculator: stub status check",
    &run_gear_train_calculator);

}
