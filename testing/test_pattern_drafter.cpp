// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1763_pattern_drafter/pattern_drafter.hpp"

namespace {

testing::TestOutcome run_pattern_drafter() {
    pattern_drafter::init();
    auto s = pattern_drafter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pattern_drafter::shutdown();
    return testing::ok();
}

const int _reg_pattern_drafter = testing::register_test(
    "pattern_drafter",
    "1763_pattern_drafter: stub status check",
    &run_pattern_drafter);

}
