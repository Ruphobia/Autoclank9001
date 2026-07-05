#include "test_runner.hpp"
#include "../modules/1145_mission_timeline_planner/mission_timeline_planner.hpp"

namespace {

testing::TestOutcome run_mission_timeline_planner() {
    mission_timeline_planner::init();
    auto s = mission_timeline_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mission_timeline_planner::shutdown();
    return testing::ok();
}

const int _reg_mission_timeline_planner = testing::register_test(
    "mission_timeline_planner",
    "1145_mission_timeline_planner: stub status check",
    &run_mission_timeline_planner);

}
