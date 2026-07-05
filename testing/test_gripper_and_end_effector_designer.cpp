#include "test_runner.hpp"
#include "../modules/1267_gripper_and_end_effector_designer/gripper_and_end_effector_designer.hpp"

namespace {

testing::TestOutcome run_gripper_and_end_effector_designer() {
    gripper_and_end_effector_designer::init();
    auto s = gripper_and_end_effector_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gripper_and_end_effector_designer::shutdown();
    return testing::ok();
}

const int _reg_gripper_and_end_effector_designer = testing::register_test(
    "gripper_and_end_effector_designer",
    "1267_gripper_and_end_effector_designer: stub status check",
    &run_gripper_and_end_effector_designer);

}
