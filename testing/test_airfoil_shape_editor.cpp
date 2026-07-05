// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1149_airfoil_shape_editor/airfoil_shape_editor.hpp"

namespace {

testing::TestOutcome run_airfoil_shape_editor() {
    airfoil_shape_editor::init();
    auto s = airfoil_shape_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    airfoil_shape_editor::shutdown();
    return testing::ok();
}

const int _reg_airfoil_shape_editor = testing::register_test(
    "airfoil_shape_editor",
    "1149_airfoil_shape_editor: stub status check",
    &run_airfoil_shape_editor);

}
