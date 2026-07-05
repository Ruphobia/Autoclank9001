// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/158_simulator_and_emulator_orchestrator/simulator_and_emulator_orchestrator.hpp"

namespace {

testing::TestOutcome run_simulator_and_emulator_orchestrator() {
    simulator_and_emulator_orchestrator::init();
    auto s = simulator_and_emulator_orchestrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    simulator_and_emulator_orchestrator::shutdown();
    return testing::ok();
}

const int _reg_simulator_and_emulator_orchestrator = testing::register_test(
    "simulator_and_emulator_orchestrator",
    "158_simulator_and_emulator_orchestrator: stub status check",
    &run_simulator_and_emulator_orchestrator);

}
