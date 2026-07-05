// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1470_air_quality_aggregator/air_quality_aggregator.hpp"

namespace {

testing::TestOutcome run_air_quality_aggregator() {
    air_quality_aggregator::init();
    auto s = air_quality_aggregator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    air_quality_aggregator::shutdown();
    return testing::ok();
}

const int _reg_air_quality_aggregator = testing::register_test(
    "air_quality_aggregator",
    "1470_air_quality_aggregator: stub status check",
    &run_air_quality_aggregator);

}
