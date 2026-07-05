// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1591_earthquake_feed/earthquake_feed.hpp"

namespace {

testing::TestOutcome run_earthquake_feed() {
    earthquake_feed::init();
    auto s = earthquake_feed::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    earthquake_feed::shutdown();
    return testing::ok();
}

const int _reg_earthquake_feed = testing::register_test(
    "earthquake_feed",
    "1591_earthquake_feed: stub status check",
    &run_earthquake_feed);

}
