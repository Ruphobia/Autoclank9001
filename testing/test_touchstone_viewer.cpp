// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1190_touchstone_viewer/touchstone_viewer.hpp"

namespace {

testing::TestOutcome run_touchstone_viewer() {
    touchstone_viewer::init();
    auto s = touchstone_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    touchstone_viewer::shutdown();
    return testing::ok();
}

const int _reg_touchstone_viewer = testing::register_test(
    "touchstone_viewer",
    "1190_touchstone_viewer: stub status check",
    &run_touchstone_viewer);

}
