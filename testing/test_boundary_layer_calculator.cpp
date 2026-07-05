// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1152_boundary_layer_calculator/boundary_layer_calculator.hpp"

namespace {

testing::TestOutcome run_boundary_layer_calculator() {
    boundary_layer_calculator::init();
    auto s = boundary_layer_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    boundary_layer_calculator::shutdown();
    return testing::ok();
}

const int _reg_boundary_layer_calculator = testing::register_test(
    "boundary_layer_calculator",
    "1152_boundary_layer_calculator: stub status check",
    &run_boundary_layer_calculator);

}
