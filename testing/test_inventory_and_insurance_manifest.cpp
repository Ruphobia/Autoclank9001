#include "test_runner.hpp"
#include "../modules/1894_inventory_and_insurance_manifest/inventory_and_insurance_manifest.hpp"

namespace {

testing::TestOutcome run_inventory_and_insurance_manifest() {
    inventory_and_insurance_manifest::init();
    auto s = inventory_and_insurance_manifest::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    inventory_and_insurance_manifest::shutdown();
    return testing::ok();
}

const int _reg_inventory_and_insurance_manifest = testing::register_test(
    "inventory_and_insurance_manifest",
    "1894_inventory_and_insurance_manifest: stub status check",
    &run_inventory_and_insurance_manifest);

}
