#include "test_runner.hpp"
#include "../modules/291_state_machine_editor/state_machine_editor.hpp"

namespace {

testing::TestOutcome run_state_machine_editor() {
    state_machine_editor::init();
    auto s = state_machine_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    state_machine_editor::shutdown();
    return testing::ok();
}

const int _reg_state_machine_editor = testing::register_test(
    "state_machine_editor",
    "291_state_machine_editor: stub status check",
    &run_state_machine_editor);

}
