// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1451_inequality_and_segregation_metrics/inequality_and_segregation_metrics.hpp"

namespace {

testing::TestOutcome run_inequality_and_segregation_metrics() {
    inequality_and_segregation_metrics::init();
    auto s = inequality_and_segregation_metrics::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    inequality_and_segregation_metrics::shutdown();
    return testing::ok();
}

const int _reg_inequality_and_segregation_metrics = testing::register_test(
    "inequality_and_segregation_metrics",
    "1451_inequality_and_segregation_metrics: stub status check",
    &run_inequality_and_segregation_metrics);

}
