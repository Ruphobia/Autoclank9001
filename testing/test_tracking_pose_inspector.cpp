#include "test_runner.hpp"
#include "../modules/311_tracking_pose_inspector/tracking_pose_inspector.hpp"

namespace {

testing::TestOutcome run_tracking_pose_inspector() {
    tracking_pose_inspector::init();
    auto s = tracking_pose_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tracking_pose_inspector::shutdown();
    return testing::ok();
}

const int _reg_tracking_pose_inspector = testing::register_test(
    "tracking_pose_inspector",
    "311_tracking_pose_inspector: stub status check",
    &run_tracking_pose_inspector);

}
