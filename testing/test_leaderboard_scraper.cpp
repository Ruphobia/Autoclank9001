// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1986_leaderboard_scraper/leaderboard_scraper.hpp"

namespace {

testing::TestOutcome run_leaderboard_scraper() {
    leaderboard_scraper::init();
    auto s = leaderboard_scraper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    leaderboard_scraper::shutdown();
    return testing::ok();
}

const int _reg_leaderboard_scraper = testing::register_test(
    "leaderboard_scraper",
    "1986_leaderboard_scraper: stub status check",
    &run_leaderboard_scraper);

}
