#include "test_runner.hpp"
#include "../modules/2113_water_filter_sizing_planner/water_filter_sizing_planner.hpp"

namespace {

testing::TestOutcome run_water_filter_sizing_planner() {
    water_filter_sizing_planner::init();
    auto s = water_filter_sizing_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    water_filter_sizing_planner::shutdown();
    return testing::ok();
}

const int _reg_water_filter_sizing_planner = testing::register_test(
    "water_filter_sizing_planner",
    "2113_water_filter_sizing_planner: stub status check",
    &run_water_filter_sizing_planner);

}
