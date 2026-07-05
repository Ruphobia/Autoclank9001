// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1844_damascus_layer_calculator/damascus_layer_calculator.hpp"

namespace {

testing::TestOutcome run_damascus_layer_calculator() {
    damascus_layer_calculator::init();
    auto s = damascus_layer_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    damascus_layer_calculator::shutdown();
    return testing::ok();
}

const int _reg_damascus_layer_calculator = testing::register_test(
    "damascus_layer_calculator",
    "1844_damascus_layer_calculator: stub status check",
    &run_damascus_layer_calculator);

}
