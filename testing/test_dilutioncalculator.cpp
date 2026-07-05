// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1826_dilutioncalculator/dilutioncalculator.hpp"

namespace {

testing::TestOutcome run_dilutioncalculator() {
    dilutioncalculator::init();
    auto s = dilutioncalculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dilutioncalculator::shutdown();
    return testing::ok();
}

const int _reg_dilutioncalculator = testing::register_test(
    "dilutioncalculator",
    "1826_dilutioncalculator: stub status check",
    &run_dilutioncalculator);

}
