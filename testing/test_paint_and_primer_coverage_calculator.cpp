// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2037_paint_and_primer_coverage_calculator/paint_and_primer_coverage_calculator.hpp"

namespace {

testing::TestOutcome run_paint_and_primer_coverage_calculator() {
    paint_and_primer_coverage_calculator::init();
    auto s = paint_and_primer_coverage_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paint_and_primer_coverage_calculator::shutdown();
    return testing::ok();
}

const int _reg_paint_and_primer_coverage_calculator = testing::register_test(
    "paint_and_primer_coverage_calculator",
    "2037_paint_and_primer_coverage_calculator: stub status check",
    &run_paint_and_primer_coverage_calculator);

}
