// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/388_evm_debugger/evm_debugger.hpp"

namespace {

testing::TestOutcome run_evm_debugger() {
    evm_debugger::init();
    auto s = evm_debugger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    evm_debugger::shutdown();
    return testing::ok();
}

const int _reg_evm_debugger = testing::register_test(
    "evm_debugger",
    "388_evm_debugger: stub status check",
    &run_evm_debugger);

}
