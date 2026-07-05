// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/750_argument_map_editor/argument_map_editor.hpp"

namespace {

testing::TestOutcome run_argument_map_editor() {
    argument_map_editor::init();
    auto s = argument_map_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    argument_map_editor::shutdown();
    return testing::ok();
}

const int _reg_argument_map_editor = testing::register_test(
    "argument_map_editor",
    "750_argument_map_editor: stub status check",
    &run_argument_map_editor);

}
