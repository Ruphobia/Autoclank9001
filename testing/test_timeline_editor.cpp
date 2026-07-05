// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/499_timeline_editor/timeline_editor.hpp"

namespace {

testing::TestOutcome run_timeline_editor() {
    timeline_editor::init();
    auto s = timeline_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    timeline_editor::shutdown();
    return testing::ok();
}

const int _reg_timeline_editor = testing::register_test(
    "timeline_editor",
    "499_timeline_editor: stub status check",
    &run_timeline_editor);

}
