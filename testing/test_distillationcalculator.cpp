// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1829_distillationcalculator/distillationcalculator.hpp"

namespace {

testing::TestOutcome run_distillationcalculator() {
    distillationcalculator::init();
    auto s = distillationcalculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    distillationcalculator::shutdown();
    return testing::ok();
}

const int _reg_distillationcalculator = testing::register_test(
    "distillationcalculator",
    "1829_distillationcalculator: stub status check",
    &run_distillationcalculator);

}
