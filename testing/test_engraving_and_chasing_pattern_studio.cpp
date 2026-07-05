// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1857_engraving_and_chasing_pattern_studio/engraving_and_chasing_pattern_studio.hpp"

namespace {

testing::TestOutcome run_engraving_and_chasing_pattern_studio() {
    engraving_and_chasing_pattern_studio::init();
    auto s = engraving_and_chasing_pattern_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    engraving_and_chasing_pattern_studio::shutdown();
    return testing::ok();
}

const int _reg_engraving_and_chasing_pattern_studio = testing::register_test(
    "engraving_and_chasing_pattern_studio",
    "1857_engraving_and_chasing_pattern_studio: stub status check",
    &run_engraving_and_chasing_pattern_studio);

}
