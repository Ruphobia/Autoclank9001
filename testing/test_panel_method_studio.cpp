#include "test_runner.hpp"
#include "../modules/1154_panel_method_studio/panel_method_studio.hpp"

namespace {

testing::TestOutcome run_panel_method_studio() {
    panel_method_studio::init();
    auto s = panel_method_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    panel_method_studio::shutdown();
    return testing::ok();
}

const int _reg_panel_method_studio = testing::register_test(
    "panel_method_studio",
    "1154_panel_method_studio: stub status check",
    &run_panel_method_studio);

}
