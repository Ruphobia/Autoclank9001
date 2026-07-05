// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1911_cue_stack_programmer/cue_stack_programmer.hpp"

namespace {

testing::TestOutcome run_cue_stack_programmer() {
    cue_stack_programmer::init();
    auto s = cue_stack_programmer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cue_stack_programmer::shutdown();
    return testing::ok();
}

const int _reg_cue_stack_programmer = testing::register_test(
    "cue_stack_programmer",
    "1911_cue_stack_programmer: stub status check",
    &run_cue_stack_programmer);

}
