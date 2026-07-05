// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1850_clay_body_formulator/clay_body_formulator.hpp"

namespace {

testing::TestOutcome run_clay_body_formulator() {
    clay_body_formulator::init();
    auto s = clay_body_formulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    clay_body_formulator::shutdown();
    return testing::ok();
}

const int _reg_clay_body_formulator = testing::register_test(
    "clay_body_formulator",
    "1850_clay_body_formulator: stub status check",
    &run_clay_body_formulator);

}
