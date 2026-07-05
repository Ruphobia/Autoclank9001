#include "test_runner.hpp"
#include "../modules/1259_slam_workbench/slam_workbench.hpp"

namespace {

testing::TestOutcome run_slam_workbench() {
    slam_workbench::init();
    auto s = slam_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    slam_workbench::shutdown();
    return testing::ok();
}

const int _reg_slam_workbench = testing::register_test(
    "slam_workbench",
    "1259_slam_workbench: stub status check",
    &run_slam_workbench);

}
