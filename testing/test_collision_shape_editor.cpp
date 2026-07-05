// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/303_collision_shape_editor/collision_shape_editor.hpp"

namespace {

testing::TestOutcome run_collision_shape_editor() {
    collision_shape_editor::init();
    auto s = collision_shape_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    collision_shape_editor::shutdown();
    return testing::ok();
}

const int _reg_collision_shape_editor = testing::register_test(
    "collision_shape_editor",
    "303_collision_shape_editor: stub status check",
    &run_collision_shape_editor);

}
