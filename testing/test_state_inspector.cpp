// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/332_state_inspector/state_inspector.hpp"

namespace {

testing::TestOutcome run_state_inspector() {
    state_inspector::init();
    auto s = state_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    state_inspector::shutdown();
    return testing::ok();
}

const int _reg_state_inspector = testing::register_test(
    "state_inspector",
    "332_state_inspector: stub status check",
    &run_state_inspector);

}
