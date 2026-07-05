#include "test_runner.hpp"
#include "../modules/1264_pose_estimation_studio/pose_estimation_studio.hpp"

namespace {

testing::TestOutcome run_pose_estimation_studio() {
    pose_estimation_studio::init();
    auto s = pose_estimation_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pose_estimation_studio::shutdown();
    return testing::ok();
}

const int _reg_pose_estimation_studio = testing::register_test(
    "pose_estimation_studio",
    "1264_pose_estimation_studio: stub status check",
    &run_pose_estimation_studio);

}
