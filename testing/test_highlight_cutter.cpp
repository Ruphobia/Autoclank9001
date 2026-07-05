// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1995_highlight_cutter/highlight_cutter.hpp"

namespace {

testing::TestOutcome run_highlight_cutter() {
    highlight_cutter::init();
    auto s = highlight_cutter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    highlight_cutter::shutdown();
    return testing::ok();
}

const int _reg_highlight_cutter = testing::register_test(
    "highlight_cutter",
    "1995_highlight_cutter: stub status check",
    &run_highlight_cutter);

}
