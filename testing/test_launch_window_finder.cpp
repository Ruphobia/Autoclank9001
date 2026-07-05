// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1135_launch_window_finder/launch_window_finder.hpp"

namespace {

testing::TestOutcome run_launch_window_finder() {
    launch_window_finder::init();
    auto s = launch_window_finder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    launch_window_finder::shutdown();
    return testing::ok();
}

const int _reg_launch_window_finder = testing::register_test(
    "launch_window_finder",
    "1135_launch_window_finder: stub status check",
    &run_launch_window_finder);

}
