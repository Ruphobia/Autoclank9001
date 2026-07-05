#include "test_runner.hpp"
#include "../modules/211_display_and_ui_prototyper/display_and_ui_prototyper.hpp"

namespace {

testing::TestOutcome run_display_and_ui_prototyper() {
    display_and_ui_prototyper::init();
    auto s = display_and_ui_prototyper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    display_and_ui_prototyper::shutdown();
    return testing::ok();
}

const int _reg_display_and_ui_prototyper = testing::register_test(
    "display_and_ui_prototyper",
    "211_display_and_ui_prototyper: stub status check",
    &run_display_and_ui_prototyper);

}
