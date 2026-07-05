// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1567_lca_inventory_builder/lca_inventory_builder.hpp"

namespace {

testing::TestOutcome run_lca_inventory_builder() {
    lca_inventory_builder::init();
    auto s = lca_inventory_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lca_inventory_builder::shutdown();
    return testing::ok();
}

const int _reg_lca_inventory_builder = testing::register_test(
    "lca_inventory_builder",
    "1567_lca_inventory_builder: stub status check",
    &run_lca_inventory_builder);

}
