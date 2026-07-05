// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2058_gpx_track_workbench/gpx_track_workbench.hpp"

namespace {

testing::TestOutcome run_gpx_track_workbench() {
    gpx_track_workbench::init();
    auto s = gpx_track_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gpx_track_workbench::shutdown();
    return testing::ok();
}

const int _reg_gpx_track_workbench = testing::register_test(
    "gpx_track_workbench",
    "2058_gpx_track_workbench: stub status check",
    &run_gpx_track_workbench);

}
