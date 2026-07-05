// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/751_formal_logic_prover/formal_logic_prover.hpp"

namespace {

testing::TestOutcome run_formal_logic_prover() {
    formal_logic_prover::init();
    auto s = formal_logic_prover::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    formal_logic_prover::shutdown();
    return testing::ok();
}

const int _reg_formal_logic_prover = testing::register_test(
    "formal_logic_prover",
    "751_formal_logic_prover: stub status check",
    &run_formal_logic_prover);

}
