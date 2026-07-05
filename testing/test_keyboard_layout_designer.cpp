#include "test_runner.hpp"
#include "../modules/824_keyboard_layout_designer/keyboard_layout_designer.hpp"

namespace {

testing::TestOutcome run_keyboard_layout_designer() {
    keyboard_layout_designer::init();
    auto s = keyboard_layout_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    keyboard_layout_designer::shutdown();
    return testing::ok();
}

const int _reg_keyboard_layout_designer = testing::register_test(
    "keyboard_layout_designer",
    "824_keyboard_layout_designer: stub status check",
    &run_keyboard_layout_designer);

}
