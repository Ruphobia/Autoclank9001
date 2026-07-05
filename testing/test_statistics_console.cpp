// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/657_statistics_console/statistics_console.hpp"

namespace {

testing::TestOutcome run_statistics_console() {
    statistics_console::init();
    auto s = statistics_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    statistics_console::shutdown();
    return testing::ok();
}

const int _reg_statistics_console = testing::register_test(
    "statistics_console",
    "657_statistics_console: stub status check",
    &run_statistics_console);

}
