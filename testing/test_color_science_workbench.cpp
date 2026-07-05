// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/885_color_science_workbench/color_science_workbench.hpp"

namespace {

testing::TestOutcome run_color_science_workbench() {
    color_science_workbench::init();
    auto s = color_science_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_science_workbench::shutdown();
    return testing::ok();
}

const int _reg_color_science_workbench = testing::register_test(
    "color_science_workbench",
    "885_color_science_workbench: stub status check",
    &run_color_science_workbench);

}
