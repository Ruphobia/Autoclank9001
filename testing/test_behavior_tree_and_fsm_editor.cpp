// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1269_behavior_tree_and_fsm_editor/behavior_tree_and_fsm_editor.hpp"

namespace {

testing::TestOutcome run_behavior_tree_and_fsm_editor() {
    behavior_tree_and_fsm_editor::init();
    auto s = behavior_tree_and_fsm_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    behavior_tree_and_fsm_editor::shutdown();
    return testing::ok();
}

const int _reg_behavior_tree_and_fsm_editor = testing::register_test(
    "behavior_tree_and_fsm_editor",
    "1269_behavior_tree_and_fsm_editor: stub status check",
    &run_behavior_tree_and_fsm_editor);

}
