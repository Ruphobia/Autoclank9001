// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1233_wiring_diagram_pinout_browser/wiring_diagram_pinout_browser.hpp"

namespace {

testing::TestOutcome run_wiring_diagram_pinout_browser() {
    wiring_diagram_pinout_browser::init();
    auto s = wiring_diagram_pinout_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wiring_diagram_pinout_browser::shutdown();
    return testing::ok();
}

const int _reg_wiring_diagram_pinout_browser = testing::register_test(
    "wiring_diagram_pinout_browser",
    "1233_wiring_diagram_pinout_browser: stub status check",
    &run_wiring_diagram_pinout_browser);

}
