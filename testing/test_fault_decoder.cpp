// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/185_fault_decoder/fault_decoder.hpp"

namespace {

testing::TestOutcome run_fault_decoder() {
    fault_decoder::init();
    auto s = fault_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fault_decoder::shutdown();
    return testing::ok();
}

const int _reg_fault_decoder = testing::register_test(
    "fault_decoder",
    "185_fault_decoder: stub status check",
    &run_fault_decoder);

}
