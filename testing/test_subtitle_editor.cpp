// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/819_subtitle_editor/subtitle_editor.hpp"

namespace {

testing::TestOutcome run_subtitle_editor() {
    subtitle_editor::init();
    auto s = subtitle_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    subtitle_editor::shutdown();
    return testing::ok();
}

const int _reg_subtitle_editor = testing::register_test(
    "subtitle_editor",
    "819_subtitle_editor: stub status check",
    &run_subtitle_editor);

}
