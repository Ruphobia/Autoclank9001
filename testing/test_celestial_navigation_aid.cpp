// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2128_celestial_navigation_aid/celestial_navigation_aid.hpp"

namespace {

testing::TestOutcome run_celestial_navigation_aid() {
    celestial_navigation_aid::init();
    auto s = celestial_navigation_aid::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    celestial_navigation_aid::shutdown();
    return testing::ok();
}

const int _reg_celestial_navigation_aid = testing::register_test(
    "celestial_navigation_aid",
    "2128_celestial_navigation_aid: stub status check",
    &run_celestial_navigation_aid);

}
