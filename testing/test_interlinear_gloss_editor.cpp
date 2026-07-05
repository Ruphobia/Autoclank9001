// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/810_interlinear_gloss_editor/interlinear_gloss_editor.hpp"

namespace {

testing::TestOutcome run_interlinear_gloss_editor() {
    interlinear_gloss_editor::init();
    auto s = interlinear_gloss_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    interlinear_gloss_editor::shutdown();
    return testing::ok();
}

const int _reg_interlinear_gloss_editor = testing::register_test(
    "interlinear_gloss_editor",
    "810_interlinear_gloss_editor: stub status check",
    &run_interlinear_gloss_editor);

}
