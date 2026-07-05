// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/349_qasm_and_format_bridge/qasm_and_format_bridge.hpp"

namespace {

testing::TestOutcome run_qasm_and_format_bridge() {
    qasm_and_format_bridge::init();
    auto s = qasm_and_format_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    qasm_and_format_bridge::shutdown();
    return testing::ok();
}

const int _reg_qasm_and_format_bridge = testing::register_test(
    "qasm_and_format_bridge",
    "349_qasm_and_format_bridge: stub status check",
    &run_qasm_and_format_bridge);

}
