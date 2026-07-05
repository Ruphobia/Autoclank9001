// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/792_finite_state_lab/finite_state_lab.hpp"

namespace {

testing::TestOutcome run_finite_state_lab() {
    finite_state_lab::init();
    auto s = finite_state_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    finite_state_lab::shutdown();
    return testing::ok();
}

const int _reg_finite_state_lab = testing::register_test(
    "finite_state_lab",
    "792_finite_state_lab: stub status check",
    &run_finite_state_lab);

}
