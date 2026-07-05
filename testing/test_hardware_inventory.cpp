// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/280_hardware_inventory/hardware_inventory.hpp"

namespace {

testing::TestOutcome run_hardware_inventory() {
    hardware_inventory::init();
    auto s = hardware_inventory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hardware_inventory::shutdown();
    return testing::ok();
}

const int _reg_hardware_inventory = testing::register_test(
    "hardware_inventory",
    "280_hardware_inventory: stub status check",
    &run_hardware_inventory);

}
