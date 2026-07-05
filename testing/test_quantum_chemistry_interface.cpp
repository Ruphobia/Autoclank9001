// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1315_quantum_chemistry_interface/quantum_chemistry_interface.hpp"

namespace {

testing::TestOutcome run_quantum_chemistry_interface() {
    quantum_chemistry_interface::init();
    auto s = quantum_chemistry_interface::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quantum_chemistry_interface::shutdown();
    return testing::ok();
}

const int _reg_quantum_chemistry_interface = testing::register_test(
    "quantum_chemistry_interface",
    "1315_quantum_chemistry_interface: stub status check",
    &run_quantum_chemistry_interface);

}
