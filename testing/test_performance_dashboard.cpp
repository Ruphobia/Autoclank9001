#include "test_runner.hpp"
#include "../modules/247_performance_dashboard/performance_dashboard.hpp"

namespace {

testing::TestOutcome run_performance_dashboard() {
    performance_dashboard::init();
    auto s = performance_dashboard::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    performance_dashboard::shutdown();
    return testing::ok();
}

const int _reg_performance_dashboard = testing::register_test(
    "performance_dashboard",
    "247_performance_dashboard: stub status check",
    &run_performance_dashboard);

}
