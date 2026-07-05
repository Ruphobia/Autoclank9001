// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/288_level_layout_studio/level_layout_studio.hpp"

namespace {

testing::TestOutcome run_level_layout_studio() {
    level_layout_studio::init();
    auto s = level_layout_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    level_layout_studio::shutdown();
    return testing::ok();
}

const int _reg_level_layout_studio = testing::register_test(
    "level_layout_studio",
    "288_level_layout_studio: stub status check",
    &run_level_layout_studio);

}
