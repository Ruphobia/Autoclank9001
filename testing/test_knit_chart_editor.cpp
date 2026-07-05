// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1771_knit_chart_editor/knit_chart_editor.hpp"

namespace {

testing::TestOutcome run_knit_chart_editor() {
    knit_chart_editor::init();
    auto s = knit_chart_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    knit_chart_editor::shutdown();
    return testing::ok();
}

const int _reg_knit_chart_editor = testing::register_test(
    "knit_chart_editor",
    "1771_knit_chart_editor: stub status check",
    &run_knit_chart_editor);

}
