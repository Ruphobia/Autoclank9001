// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2066_geocache_hunter/geocache_hunter.hpp"

namespace {

testing::TestOutcome run_geocache_hunter() {
    geocache_hunter::init();
    auto s = geocache_hunter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geocache_hunter::shutdown();
    return testing::ok();
}

const int _reg_geocache_hunter = testing::register_test(
    "geocache_hunter",
    "2066_geocache_hunter: stub status check",
    &run_geocache_hunter);

}
