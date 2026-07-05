#include "test_runner.hpp"
#include "../modules/1673_camera_move_planner/camera_move_planner.hpp"

namespace {

testing::TestOutcome run_camera_move_planner() {
    camera_move_planner::init();
    auto s = camera_move_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    camera_move_planner::shutdown();
    return testing::ok();
}

const int _reg_camera_move_planner = testing::register_test(
    "camera_move_planner",
    "1673_camera_move_planner: stub status check",
    &run_camera_move_planner);

}
