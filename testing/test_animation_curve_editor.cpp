#include "test_runner.hpp"
#include "../modules/295_animation_curve_editor/animation_curve_editor.hpp"

namespace {

testing::TestOutcome run_animation_curve_editor() {
    animation_curve_editor::init();
    auto s = animation_curve_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    animation_curve_editor::shutdown();
    return testing::ok();
}

const int _reg_animation_curve_editor = testing::register_test(
    "animation_curve_editor",
    "295_animation_curve_editor: stub status check",
    &run_animation_curve_editor);

}
