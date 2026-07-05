#include "test_runner.hpp"
#include "../modules/1168_flight_planner/flight_planner.hpp"

namespace {

testing::TestOutcome run_flight_planner() {
    flight_planner::init();
    auto s = flight_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    flight_planner::shutdown();
    return testing::ok();
}

const int _reg_flight_planner = testing::register_test(
    "flight_planner",
    "1168_flight_planner: stub status check",
    &run_flight_planner);

}
