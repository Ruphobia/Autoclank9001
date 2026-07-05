// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/173_snapshot_test_diff_viewer/snapshot_test_diff_viewer.hpp"

namespace {

testing::TestOutcome run_snapshot_test_diff_viewer() {
    snapshot_test_diff_viewer::init();
    auto s = snapshot_test_diff_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    snapshot_test_diff_viewer::shutdown();
    return testing::ok();
}

const int _reg_snapshot_test_diff_viewer = testing::register_test(
    "snapshot_test_diff_viewer",
    "173_snapshot_test_diff_viewer: stub status check",
    &run_snapshot_test_diff_viewer);

}
