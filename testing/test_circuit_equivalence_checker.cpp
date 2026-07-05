// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/348_circuit_equivalence_checker/circuit_equivalence_checker.hpp"

namespace {

testing::TestOutcome run_circuit_equivalence_checker() {
    circuit_equivalence_checker::init();
    auto s = circuit_equivalence_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    circuit_equivalence_checker::shutdown();
    return testing::ok();
}

const int _reg_circuit_equivalence_checker = testing::register_test(
    "circuit_equivalence_checker",
    "348_circuit_equivalence_checker: stub status check",
    &run_circuit_equivalence_checker);

}
