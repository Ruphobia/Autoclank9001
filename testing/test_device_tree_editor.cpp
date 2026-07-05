#include "test_runner.hpp"
#include "../modules/219_device_tree_editor/device_tree_editor.hpp"

namespace {

testing::TestOutcome run_device_tree_editor() {
    device_tree_editor::init();
    auto s = device_tree_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    device_tree_editor::shutdown();
    return testing::ok();
}

const int _reg_device_tree_editor = testing::register_test(
    "device_tree_editor",
    "219_device_tree_editor: stub status check",
    &run_device_tree_editor);

}
