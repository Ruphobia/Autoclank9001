// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1275_module_i_v_curve_simulator/module_i_v_curve_simulator.hpp"

namespace {

testing::TestOutcome run_module_i_v_curve_simulator() {
    module_i_v_curve_simulator::init();
    auto s = module_i_v_curve_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    module_i_v_curve_simulator::shutdown();
    return testing::ok();
}

const int _reg_module_i_v_curve_simulator = testing::register_test(
    "module_i_v_curve_simulator",
    "1275_module_i_v_curve_simulator: stub status check",
    &run_module_i_v_curve_simulator);

}
