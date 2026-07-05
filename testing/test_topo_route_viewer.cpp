// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2057_topo_route_viewer/topo_route_viewer.hpp"

namespace {

testing::TestOutcome run_topo_route_viewer() {
    topo_route_viewer::init();
    auto s = topo_route_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    topo_route_viewer::shutdown();
    return testing::ok();
}

const int _reg_topo_route_viewer = testing::register_test(
    "topo_route_viewer",
    "2057_topo_route_viewer: stub status check",
    &run_topo_route_viewer);

}
