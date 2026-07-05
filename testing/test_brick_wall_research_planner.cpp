#include "test_runner.hpp"
#include "../modules/2147_brick_wall_research_planner/brick_wall_research_planner.hpp"

namespace {

testing::TestOutcome run_brick_wall_research_planner() {
    brick_wall_research_planner::init();
    auto s = brick_wall_research_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    brick_wall_research_planner::shutdown();
    return testing::ok();
}

const int _reg_brick_wall_research_planner = testing::register_test(
    "brick_wall_research_planner",
    "2147_brick_wall_research_planner: stub status check",
    &run_brick_wall_research_planner);

}
