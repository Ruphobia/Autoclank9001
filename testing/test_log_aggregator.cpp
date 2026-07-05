// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/259_log_aggregator/log_aggregator.hpp"

namespace {

testing::TestOutcome run_log_aggregator() {
    log_aggregator::init();
    auto s = log_aggregator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    log_aggregator::shutdown();
    return testing::ok();
}

const int _reg_log_aggregator = testing::register_test(
    "log_aggregator",
    "259_log_aggregator: stub status check",
    &run_log_aggregator);

}
