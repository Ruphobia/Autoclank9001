#include "test_runner.hpp"
#include "../modules/2106_sail_and_rig_inventory/sail_and_rig_inventory.hpp"

namespace {

testing::TestOutcome run_sail_and_rig_inventory() {
    sail_and_rig_inventory::init();
    auto s = sail_and_rig_inventory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sail_and_rig_inventory::shutdown();
    return testing::ok();
}

const int _reg_sail_and_rig_inventory = testing::register_test(
    "sail_and_rig_inventory",
    "2106_sail_and_rig_inventory: stub status check",
    &run_sail_and_rig_inventory);

}
