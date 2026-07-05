#include "test_runner.hpp"
#include "../modules/456_sla_and_cycle_time_analytics/sla_and_cycle_time_analytics.hpp"

namespace {

testing::TestOutcome run_sla_and_cycle_time_analytics() {
    sla_and_cycle_time_analytics::init();
    auto s = sla_and_cycle_time_analytics::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sla_and_cycle_time_analytics::shutdown();
    return testing::ok();
}

const int _reg_sla_and_cycle_time_analytics = testing::register_test(
    "sla_and_cycle_time_analytics",
    "456_sla_and_cycle_time_analytics: stub status check",
    &run_sla_and_cycle_time_analytics);

}
