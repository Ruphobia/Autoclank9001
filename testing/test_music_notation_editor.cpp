// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1741_music_notation_editor/music_notation_editor.hpp"

namespace {

testing::TestOutcome run_music_notation_editor() {
    music_notation_editor::init();
    auto s = music_notation_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    music_notation_editor::shutdown();
    return testing::ok();
}

const int _reg_music_notation_editor = testing::register_test(
    "music_notation_editor",
    "1741_music_notation_editor: stub status check",
    &run_music_notation_editor);

}
