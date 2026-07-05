// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/976_feeds_speeds_and_tool_library/feeds_speeds_and_tool_library.hpp"

namespace {

testing::TestOutcome run_feeds_speeds_and_tool_library() {
    feeds_speeds_and_tool_library::init();
    auto s = feeds_speeds_and_tool_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    feeds_speeds_and_tool_library::shutdown();
    return testing::ok();
}

const int _reg_feeds_speeds_and_tool_library = testing::register_test(
    "feeds_speeds_and_tool_library",
    "976_feeds_speeds_and_tool_library: stub status check",
    &run_feeds_speeds_and_tool_library);

}
