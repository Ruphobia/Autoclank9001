// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/550_stocks_and_options_analytics/stocks_and_options_analytics.hpp"

namespace {

testing::TestOutcome run_stocks_and_options_analytics() {
    stocks_and_options_analytics::init();
    auto s = stocks_and_options_analytics::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stocks_and_options_analytics::shutdown();
    return testing::ok();
}

const int _reg_stocks_and_options_analytics = testing::register_test(
    "stocks_and_options_analytics",
    "550_stocks_and_options_analytics: stub status check",
    &run_stocks_and_options_analytics);

}
