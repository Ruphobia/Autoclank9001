// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1026_hydraulic_circuit_designer/hydraulic_circuit_designer.hpp"

namespace {

testing::TestOutcome run_hydraulic_circuit_designer() {
    hydraulic_circuit_designer::init();
    auto s = hydraulic_circuit_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hydraulic_circuit_designer::shutdown();
    return testing::ok();
}

const int _reg_hydraulic_circuit_designer = testing::register_test(
    "hydraulic_circuit_designer",
    "1026_hydraulic_circuit_designer: stub status check",
    &run_hydraulic_circuit_designer);

}
