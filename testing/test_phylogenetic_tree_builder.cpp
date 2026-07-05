// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1325_phylogenetic_tree_builder/phylogenetic_tree_builder.hpp"

namespace {

testing::TestOutcome run_phylogenetic_tree_builder() {
    phylogenetic_tree_builder::init();
    auto s = phylogenetic_tree_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phylogenetic_tree_builder::shutdown();
    return testing::ok();
}

const int _reg_phylogenetic_tree_builder = testing::register_test(
    "phylogenetic_tree_builder",
    "1325_phylogenetic_tree_builder: stub status check",
    &run_phylogenetic_tree_builder);

}
