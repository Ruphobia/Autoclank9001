// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/337_pulse_schedule_editor/pulse_schedule_editor.hpp"

namespace {

testing::TestOutcome run_pulse_schedule_editor() {
    pulse_schedule_editor::init();
    auto s = pulse_schedule_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pulse_schedule_editor::shutdown();
    return testing::ok();
}

const int _reg_pulse_schedule_editor = testing::register_test(
    "pulse_schedule_editor",
    "337_pulse_schedule_editor: stub status check",
    &run_pulse_schedule_editor);

}
