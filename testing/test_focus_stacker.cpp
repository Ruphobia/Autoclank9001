// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1674_focus_stacker/focus_stacker.hpp"

namespace {

testing::TestOutcome run_focus_stacker() {
    focus_stacker::init();
    auto s = focus_stacker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    focus_stacker::shutdown();
    return testing::ok();
}

const int _reg_focus_stacker = testing::register_test(
    "focus_stacker",
    "1674_focus_stacker: stub status check",
    &run_focus_stacker);

}
