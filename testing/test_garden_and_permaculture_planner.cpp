#include "test_runner.hpp"
#include "../modules/2120_garden_and_permaculture_planner/garden_and_permaculture_planner.hpp"

namespace {

testing::TestOutcome run_garden_and_permaculture_planner() {
    garden_and_permaculture_planner::init();
    auto s = garden_and_permaculture_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    garden_and_permaculture_planner::shutdown();
    return testing::ok();
}

const int _reg_garden_and_permaculture_planner = testing::register_test(
    "garden_and_permaculture_planner",
    "2120_garden_and_permaculture_planner: stub status check",
    &run_garden_and_permaculture_planner);

}
