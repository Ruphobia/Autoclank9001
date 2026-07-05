// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/402_emulator_harness/emulator_harness.hpp"

namespace {

testing::TestOutcome run_emulator_harness() {
    emulator_harness::init();
    auto s = emulator_harness::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    emulator_harness::shutdown();
    return testing::ok();
}

const int _reg_emulator_harness = testing::register_test(
    "emulator_harness",
    "402_emulator_harness: stub status check",
    &run_emulator_harness);

}
