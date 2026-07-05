// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1831_carbonationcalculator/carbonationcalculator.hpp"

namespace {

testing::TestOutcome run_carbonationcalculator() {
    carbonationcalculator::init();
    auto s = carbonationcalculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    carbonationcalculator::shutdown();
    return testing::ok();
}

const int _reg_carbonationcalculator = testing::register_test(
    "carbonationcalculator",
    "1831_carbonationcalculator: stub status check",
    &run_carbonationcalculator);

}
