// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/196_rtos_inspector/rtos_inspector.hpp"

namespace {

testing::TestOutcome run_rtos_inspector() {
    rtos_inspector::init();
    auto s = rtos_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rtos_inspector::shutdown();
    return testing::ok();
}

const int _reg_rtos_inspector = testing::register_test(
    "rtos_inspector",
    "196_rtos_inspector: stub status check",
    &run_rtos_inspector);

}
