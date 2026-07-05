// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/920_latex_formula_editor/latex_formula_editor.hpp"

namespace {

testing::TestOutcome run_latex_formula_editor() {
    latex_formula_editor::init();
    auto s = latex_formula_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    latex_formula_editor::shutdown();
    return testing::ok();
}

const int _reg_latex_formula_editor = testing::register_test(
    "latex_formula_editor",
    "920_latex_formula_editor: stub status check",
    &run_latex_formula_editor);

}
