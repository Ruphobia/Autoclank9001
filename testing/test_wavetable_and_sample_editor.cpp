// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1758_wavetable_and_sample_editor/wavetable_and_sample_editor.hpp"

namespace {

testing::TestOutcome run_wavetable_and_sample_editor() {
    wavetable_and_sample_editor::init();
    auto s = wavetable_and_sample_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wavetable_and_sample_editor::shutdown();
    return testing::ok();
}

const int _reg_wavetable_and_sample_editor = testing::register_test(
    "wavetable_and_sample_editor",
    "1758_wavetable_and_sample_editor: stub status check",
    &run_wavetable_and_sample_editor);

}
