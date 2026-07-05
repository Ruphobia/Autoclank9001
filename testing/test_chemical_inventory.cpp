#include "test_runner.hpp"
#include "../modules/1308_chemical_inventory/chemical_inventory.hpp"

namespace {

testing::TestOutcome run_chemical_inventory() {
    chemical_inventory::init();
    auto s = chemical_inventory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chemical_inventory::shutdown();
    return testing::ok();
}

const int _reg_chemical_inventory = testing::register_test(
    "chemical_inventory",
    "1308_chemical_inventory: stub status check",
    &run_chemical_inventory);

}
