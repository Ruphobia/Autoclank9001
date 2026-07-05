// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1822_extractioncalculator/extractioncalculator.hpp"

namespace {

testing::TestOutcome run_extractioncalculator() {
    extractioncalculator::init();
    auto s = extractioncalculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    extractioncalculator::shutdown();
    return testing::ok();
}

const int _reg_extractioncalculator = testing::register_test(
    "extractioncalculator",
    "1822_extractioncalculator: stub status check",
    &run_extractioncalculator);

}
