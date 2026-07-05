// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/207_disassembler_and_binary_diff/disassembler_and_binary_diff.hpp"

namespace {

testing::TestOutcome run_disassembler_and_binary_diff() {
    disassembler_and_binary_diff::init();
    auto s = disassembler_and_binary_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    disassembler_and_binary_diff::shutdown();
    return testing::ok();
}

const int _reg_disassembler_and_binary_diff = testing::register_test(
    "disassembler_and_binary_diff",
    "207_disassembler_and_binary_diff: stub status check",
    &run_disassembler_and_binary_diff);

}
