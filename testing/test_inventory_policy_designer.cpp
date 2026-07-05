#include "test_runner.hpp"
#include "../modules/480_inventory_policy_designer/inventory_policy_designer.hpp"

namespace {

testing::TestOutcome run_inventory_policy_designer() {
    inventory_policy_designer::init();
    auto s = inventory_policy_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    inventory_policy_designer::shutdown();
    return testing::ok();
}

const int _reg_inventory_policy_designer = testing::register_test(
    "inventory_policy_designer",
    "480_inventory_policy_designer: stub status check",
    &run_inventory_policy_designer);

}
