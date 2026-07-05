// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1191_spectrum_analyzer_console/spectrum_analyzer_console.hpp"

namespace {

testing::TestOutcome run_spectrum_analyzer_console() {
    spectrum_analyzer_console::init();
    auto s = spectrum_analyzer_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrum_analyzer_console::shutdown();
    return testing::ok();
}

const int _reg_spectrum_analyzer_console = testing::register_test(
    "spectrum_analyzer_console",
    "1191_spectrum_analyzer_console: stub status check",
    &run_spectrum_analyzer_console);

}
