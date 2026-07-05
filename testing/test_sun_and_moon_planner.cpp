#include "test_runner.hpp"
#include "../modules/1668_sun_and_moon_planner/sun_and_moon_planner.hpp"

namespace {

testing::TestOutcome run_sun_and_moon_planner() {
    sun_and_moon_planner::init();
    auto s = sun_and_moon_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sun_and_moon_planner::shutdown();
    return testing::ok();
}

const int _reg_sun_and_moon_planner = testing::register_test(
    "sun_and_moon_planner",
    "1668_sun_and_moon_planner: stub status check",
    &run_sun_and_moon_planner);

}
