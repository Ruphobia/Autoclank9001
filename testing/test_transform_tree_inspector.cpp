// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1260_transform_tree_inspector/transform_tree_inspector.hpp"

namespace {

testing::TestOutcome run_transform_tree_inspector() {
    transform_tree_inspector::init();
    auto s = transform_tree_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transform_tree_inspector::shutdown();
    return testing::ok();
}

const int _reg_transform_tree_inspector = testing::register_test(
    "transform_tree_inspector",
    "1260_transform_tree_inspector: stub status check",
    &run_transform_tree_inspector);

}
