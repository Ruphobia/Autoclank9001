// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/606_watchlist_screener/watchlist_screener.hpp"

namespace {

testing::TestOutcome run_watchlist_screener() {
    watchlist_screener::init();
    auto s = watchlist_screener::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    watchlist_screener::shutdown();
    return testing::ok();
}

const int _reg_watchlist_screener = testing::register_test(
    "watchlist_screener",
    "606_watchlist_screener: stub status check",
    &run_watchlist_screener);

}
