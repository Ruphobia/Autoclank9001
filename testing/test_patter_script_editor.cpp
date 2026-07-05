// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1939_patter_script_editor/patter_script_editor.hpp"

namespace {

testing::TestOutcome run_patter_script_editor() {
    patter_script_editor::init();
    auto s = patter_script_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patter_script_editor::shutdown();
    return testing::ok();
}

const int _reg_patter_script_editor = testing::register_test(
    "patter_script_editor",
    "1939_patter_script_editor: stub status check",
    &run_patter_script_editor);

}
