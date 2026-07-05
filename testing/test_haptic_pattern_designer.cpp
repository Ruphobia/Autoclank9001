// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/322_haptic_pattern_designer/haptic_pattern_designer.hpp"

namespace {

testing::TestOutcome run_haptic_pattern_designer() {
    haptic_pattern_designer::init();
    auto s = haptic_pattern_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    haptic_pattern_designer::shutdown();
    return testing::ok();
}

const int _reg_haptic_pattern_designer = testing::register_test(
    "haptic_pattern_designer",
    "322_haptic_pattern_designer: stub status check",
    &run_haptic_pattern_designer);

}
