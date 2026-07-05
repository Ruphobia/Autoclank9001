// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1320_sequence_editor/sequence_editor.hpp"

namespace {

testing::TestOutcome run_sequence_editor() {
    sequence_editor::init();
    auto s = sequence_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sequence_editor::shutdown();
    return testing::ok();
}

const int _reg_sequence_editor = testing::register_test(
    "sequence_editor",
    "1320_sequence_editor: stub status check",
    &run_sequence_editor);

}
