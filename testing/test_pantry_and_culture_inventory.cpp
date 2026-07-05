#include "test_runner.hpp"
#include "../modules/1819_pantry_and_culture_inventory/pantry_and_culture_inventory.hpp"

namespace {

testing::TestOutcome run_pantry_and_culture_inventory() {
    pantry_and_culture_inventory::init();
    auto s = pantry_and_culture_inventory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pantry_and_culture_inventory::shutdown();
    return testing::ok();
}

const int _reg_pantry_and_culture_inventory = testing::register_test(
    "pantry_and_culture_inventory",
    "1819_pantry_and_culture_inventory: stub status check",
    &run_pantry_and_culture_inventory);

}
