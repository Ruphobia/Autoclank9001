// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1993_rng_manipulator/rng_manipulator.hpp"

namespace {

testing::TestOutcome run_rng_manipulator() {
    rng_manipulator::init();
    auto s = rng_manipulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rng_manipulator::shutdown();
    return testing::ok();
}

const int _reg_rng_manipulator = testing::register_test(
    "rng_manipulator",
    "1993_rng_manipulator: stub status check",
    &run_rng_manipulator);

}
