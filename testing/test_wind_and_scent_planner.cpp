#include "test_runner.hpp"
#include "../modules/2050_wind_and_scent_planner/wind_and_scent_planner.hpp"

namespace {

testing::TestOutcome run_wind_and_scent_planner() {
    wind_and_scent_planner::init();
    auto s = wind_and_scent_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wind_and_scent_planner::shutdown();
    return testing::ok();
}

const int _reg_wind_and_scent_planner = testing::register_test(
    "wind_and_scent_planner",
    "2050_wind_and_scent_planner: stub status check",
    &run_wind_and_scent_planner);

}
