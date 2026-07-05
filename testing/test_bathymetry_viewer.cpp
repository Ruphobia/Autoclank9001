// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1541_bathymetry_viewer/bathymetry_viewer.hpp"

namespace {

testing::TestOutcome run_bathymetry_viewer() {
    bathymetry_viewer::init();
    auto s = bathymetry_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bathymetry_viewer::shutdown();
    return testing::ok();
}

const int _reg_bathymetry_viewer = testing::register_test(
    "bathymetry_viewer",
    "1541_bathymetry_viewer: stub status check",
    &run_bathymetry_viewer);

}
