// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1198_pre_flight_briefing_aggregator/pre_flight_briefing_aggregator.hpp"

namespace {

testing::TestOutcome run_pre_flight_briefing_aggregator() {
    pre_flight_briefing_aggregator::init();
    auto s = pre_flight_briefing_aggregator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pre_flight_briefing_aggregator::shutdown();
    return testing::ok();
}

const int _reg_pre_flight_briefing_aggregator = testing::register_test(
    "pre_flight_briefing_aggregator",
    "1198_pre_flight_briefing_aggregator: stub status check",
    &run_pre_flight_briefing_aggregator);

}
