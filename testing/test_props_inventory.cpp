// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1934_props_inventory/props_inventory.hpp"

namespace {

testing::TestOutcome run_props_inventory() {
    props_inventory::init();
    auto s = props_inventory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    props_inventory::shutdown();
    return testing::ok();
}

const int _reg_props_inventory = testing::register_test(
    "props_inventory",
    "1934_props_inventory: stub status check",
    &run_props_inventory);

}
