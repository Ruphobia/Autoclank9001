// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1163_compressible_flow_calculator/compressible_flow_calculator.hpp"

namespace {

testing::TestOutcome run_compressible_flow_calculator() {
    compressible_flow_calculator::init();
    auto s = compressible_flow_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compressible_flow_calculator::shutdown();
    return testing::ok();
}

const int _reg_compressible_flow_calculator = testing::register_test(
    "compressible_flow_calculator",
    "1163_compressible_flow_calculator: stub status check",
    &run_compressible_flow_calculator);

}
