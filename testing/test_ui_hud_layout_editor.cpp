#include "test_runner.hpp"
#include "../modules/307_ui_hud_layout_editor/ui_hud_layout_editor.hpp"

namespace {

testing::TestOutcome run_ui_hud_layout_editor() {
    ui_hud_layout_editor::init();
    auto s = ui_hud_layout_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ui_hud_layout_editor::shutdown();
    return testing::ok();
}

const int _reg_ui_hud_layout_editor = testing::register_test(
    "ui_hud_layout_editor",
    "307_ui_hud_layout_editor: stub status check",
    &run_ui_hud_layout_editor);

}
