// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2010_stocking_and_bioload_calculator/stocking_and_bioload_calculator.hpp"

namespace {

testing::TestOutcome run_stocking_and_bioload_calculator() {
    stocking_and_bioload_calculator::init();
    auto s = stocking_and_bioload_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stocking_and_bioload_calculator::shutdown();
    return testing::ok();
}

const int _reg_stocking_and_bioload_calculator = testing::register_test(
    "stocking_and_bioload_calculator",
    "2010_stocking_and_bioload_calculator: stub status check",
    &run_stocking_and_bioload_calculator);

}
