#include "test_runner.hpp"
#include "../modules/300_save_format_editor/save_format_editor.hpp"

namespace {

testing::TestOutcome run_save_format_editor() {
    save_format_editor::init();
    auto s = save_format_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    save_format_editor::shutdown();
    return testing::ok();
}

const int _reg_save_format_editor = testing::register_test(
    "save_format_editor",
    "300_save_format_editor: stub status check",
    &run_save_format_editor);

}
