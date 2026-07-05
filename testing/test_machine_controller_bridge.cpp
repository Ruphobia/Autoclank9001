// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/955_machine_controller_bridge/machine_controller_bridge.hpp"

namespace {

testing::TestOutcome run_machine_controller_bridge() {
    machine_controller_bridge::init();
    auto s = machine_controller_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    machine_controller_bridge::shutdown();
    return testing::ok();
}

const int _reg_machine_controller_bridge = testing::register_test(
    "machine_controller_bridge",
    "955_machine_controller_bridge: stub status check",
    &run_machine_controller_bridge);

}
