// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2129_land_navigation_toolkit/land_navigation_toolkit.hpp"

namespace {

testing::TestOutcome run_land_navigation_toolkit() {
    land_navigation_toolkit::init();
    auto s = land_navigation_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    land_navigation_toolkit::shutdown();
    return testing::ok();
}

const int _reg_land_navigation_toolkit = testing::register_test(
    "land_navigation_toolkit",
    "2129_land_navigation_toolkit: stub status check",
    &run_land_navigation_toolkit);

}
