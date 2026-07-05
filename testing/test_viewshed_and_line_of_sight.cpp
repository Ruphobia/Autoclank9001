// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1592_viewshed_and_line_of_sight/viewshed_and_line_of_sight.hpp"

namespace {

testing::TestOutcome run_viewshed_and_line_of_sight() {
    viewshed_and_line_of_sight::init();
    auto s = viewshed_and_line_of_sight::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    viewshed_and_line_of_sight::shutdown();
    return testing::ok();
}

const int _reg_viewshed_and_line_of_sight = testing::register_test(
    "viewshed_and_line_of_sight",
    "1592_viewshed_and_line_of_sight: stub status check",
    &run_viewshed_and_line_of_sight);

}
