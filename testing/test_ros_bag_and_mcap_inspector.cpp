#include "test_runner.hpp"
#include "../modules/1261_ros_bag_and_mcap_inspector/ros_bag_and_mcap_inspector.hpp"

namespace {

testing::TestOutcome run_ros_bag_and_mcap_inspector() {
    ros_bag_and_mcap_inspector::init();
    auto s = ros_bag_and_mcap_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ros_bag_and_mcap_inspector::shutdown();
    return testing::ok();
}

const int _reg_ros_bag_and_mcap_inspector = testing::register_test(
    "ros_bag_and_mcap_inspector",
    "1261_ros_bag_and_mcap_inspector: stub status check",
    &run_ros_bag_and_mcap_inspector);

}
