// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/431_equation_editor/equation_editor.hpp"

namespace {

testing::TestOutcome run_equation_editor() {
    equation_editor::init();
    auto s = equation_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    equation_editor::shutdown();
    return testing::ok();
}

const int _reg_equation_editor = testing::register_test(
    "equation_editor",
    "431_equation_editor: stub status check",
    &run_equation_editor);

}
