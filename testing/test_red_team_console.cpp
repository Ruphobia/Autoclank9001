// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/676_red_team_console/red_team_console.hpp"

namespace {

testing::TestOutcome run_red_team_console() {
    red_team_console::init();
    auto s = red_team_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    red_team_console::shutdown();
    return testing::ok();
}

const int _reg_red_team_console = testing::register_test(
    "red_team_console",
    "676_red_team_console: stub status check",
    &run_red_team_console);

}
