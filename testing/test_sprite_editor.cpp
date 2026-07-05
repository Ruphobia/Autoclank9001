#include "test_runner.hpp"
#include "../modules/285_sprite_editor/sprite_editor.hpp"

namespace {

testing::TestOutcome run_sprite_editor() {
    sprite_editor::init();
    auto s = sprite_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sprite_editor::shutdown();
    return testing::ok();
}

const int _reg_sprite_editor = testing::register_test(
    "sprite_editor",
    "285_sprite_editor: stub status check",
    &run_sprite_editor);

}
