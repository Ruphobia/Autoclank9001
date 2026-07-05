// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/087_code_editor/code_editor.hpp"

namespace {

testing::TestOutcome run_code_editor() {
    code_editor::init();
    auto s = code_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    code_editor::shutdown();
    return testing::ok();
}

const int _reg_code_editor = testing::register_test(
    "code_editor",
    "087_code_editor: stub status check",
    &run_code_editor);

}
