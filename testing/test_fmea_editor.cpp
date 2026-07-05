// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/460_fmea_editor/fmea_editor.hpp"

namespace {

testing::TestOutcome run_fmea_editor() {
    fmea_editor::init();
    auto s = fmea_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fmea_editor::shutdown();
    return testing::ok();
}

const int _reg_fmea_editor = testing::register_test(
    "fmea_editor",
    "460_fmea_editor: stub status check",
    &run_fmea_editor);

}
