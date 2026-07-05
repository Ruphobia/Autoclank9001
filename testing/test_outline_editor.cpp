// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/415_outline_editor/outline_editor.hpp"

namespace {

testing::TestOutcome run_outline_editor() {
    outline_editor::init();
    auto s = outline_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    outline_editor::shutdown();
    return testing::ok();
}

const int _reg_outline_editor = testing::register_test(
    "outline_editor",
    "415_outline_editor: stub status check",
    &run_outline_editor);

}
