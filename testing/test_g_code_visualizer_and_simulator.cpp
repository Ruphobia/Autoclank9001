// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/977_g_code_visualizer_and_simulator/g_code_visualizer_and_simulator.hpp"

namespace {

testing::TestOutcome run_g_code_visualizer_and_simulator() {
    g_code_visualizer_and_simulator::init();
    auto s = g_code_visualizer_and_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    g_code_visualizer_and_simulator::shutdown();
    return testing::ok();
}

const int _reg_g_code_visualizer_and_simulator = testing::register_test(
    "g_code_visualizer_and_simulator",
    "977_g_code_visualizer_and_simulator: stub status check",
    &run_g_code_visualizer_and_simulator);

}
