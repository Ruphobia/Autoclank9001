// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/712_critical_apparatus_editor/critical_apparatus_editor.hpp"

namespace {

testing::TestOutcome run_critical_apparatus_editor() {
    critical_apparatus_editor::init();
    auto s = critical_apparatus_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    critical_apparatus_editor::shutdown();
    return testing::ok();
}

const int _reg_critical_apparatus_editor = testing::register_test(
    "critical_apparatus_editor",
    "712_critical_apparatus_editor: stub status check",
    &run_critical_apparatus_editor);

}
