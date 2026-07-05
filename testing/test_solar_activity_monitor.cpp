#include "test_runner.hpp"
#include "../modules/1490_solar_activity_monitor/solar_activity_monitor.hpp"

namespace {

testing::TestOutcome run_solar_activity_monitor() {
    solar_activity_monitor::init();
    auto s = solar_activity_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    solar_activity_monitor::shutdown();
    return testing::ok();
}

const int _reg_solar_activity_monitor = testing::register_test(
    "solar_activity_monitor",
    "1490_solar_activity_monitor: stub status check",
    &run_solar_activity_monitor);

}
