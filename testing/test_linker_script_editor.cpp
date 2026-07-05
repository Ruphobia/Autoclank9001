#include "test_runner.hpp"
#include "../modules/191_linker_script_editor/linker_script_editor.hpp"

namespace {

testing::TestOutcome run_linker_script_editor() {
    linker_script_editor::init();
    auto s = linker_script_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    linker_script_editor::shutdown();
    return testing::ok();
}

const int _reg_linker_script_editor = testing::register_test(
    "linker_script_editor",
    "191_linker_script_editor: stub status check",
    &run_linker_script_editor);

}
