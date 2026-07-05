// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1594_borehole_log_viewer/borehole_log_viewer.hpp"

namespace {

testing::TestOutcome run_borehole_log_viewer() {
    borehole_log_viewer::init();
    auto s = borehole_log_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    borehole_log_viewer::shutdown();
    return testing::ok();
}

const int _reg_borehole_log_viewer = testing::register_test(
    "borehole_log_viewer",
    "1594_borehole_log_viewer: stub status check",
    &run_borehole_log_viewer);

}
