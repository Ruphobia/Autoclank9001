#include "test_runner.hpp"
#include "../modules/1913_console_syntax_editor/console_syntax_editor.hpp"

namespace {

testing::TestOutcome run_console_syntax_editor() {
    console_syntax_editor::init();
    auto s = console_syntax_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    console_syntax_editor::shutdown();
    return testing::ok();
}

const int _reg_console_syntax_editor = testing::register_test(
    "console_syntax_editor",
    "1913_console_syntax_editor: stub status check",
    &run_console_syntax_editor);

}
