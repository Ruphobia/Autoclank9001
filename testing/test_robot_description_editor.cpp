#include "test_runner.hpp"
#include "../modules/1246_robot_description_editor/robot_description_editor.hpp"

namespace {

testing::TestOutcome run_robot_description_editor() {
    robot_description_editor::init();
    auto s = robot_description_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    robot_description_editor::shutdown();
    return testing::ok();
}

const int _reg_robot_description_editor = testing::register_test(
    "robot_description_editor",
    "1246_robot_description_editor: stub status check",
    &run_robot_description_editor);

}
