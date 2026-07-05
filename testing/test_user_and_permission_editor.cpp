#include "test_runner.hpp"
#include "../modules/284_user_and_permission_editor/user_and_permission_editor.hpp"

namespace {

testing::TestOutcome run_user_and_permission_editor() {
    user_and_permission_editor::init();
    auto s = user_and_permission_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    user_and_permission_editor::shutdown();
    return testing::ok();
}

const int _reg_user_and_permission_editor = testing::register_test(
    "user_and_permission_editor",
    "284_user_and_permission_editor: stub status check",
    &run_user_and_permission_editor);

}
