// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/077_headless_browser_fetcher/headless_browser_fetcher.hpp"

namespace {

testing::TestOutcome run_headless_browser_fetcher() {
    headless_browser_fetcher::init();
    auto s = headless_browser_fetcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    headless_browser_fetcher::shutdown();
    return testing::ok();
}

const int _reg_headless_browser_fetcher = testing::register_test(
    "headless_browser_fetcher",
    "077_headless_browser_fetcher: stub status check",
    &run_headless_browser_fetcher);

}
