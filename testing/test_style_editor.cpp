#include "test_runner.hpp"
#include "../modules/083_style_editor/style_editor.hpp"

namespace {

testing::TestOutcome run_style_editor() {
    style_editor::init();
    auto s = style_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    style_editor::shutdown();
    return testing::ok();
}

const int _reg_style_editor = testing::register_test(
    "style_editor",
    "083_style_editor: stub status check",
    &run_style_editor);

}
