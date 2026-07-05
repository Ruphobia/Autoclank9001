// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1353_plasmid_editor/plasmid_editor.hpp"

namespace {

testing::TestOutcome run_plasmid_editor() {
    plasmid_editor::init();
    auto s = plasmid_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plasmid_editor::shutdown();
    return testing::ok();
}

const int _reg_plasmid_editor = testing::register_test(
    "plasmid_editor",
    "1353_plasmid_editor: stub status check",
    &run_plasmid_editor);

}
