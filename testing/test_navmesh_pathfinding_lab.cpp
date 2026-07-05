// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/302_navmesh_pathfinding_lab/navmesh_pathfinding_lab.hpp"

namespace {

testing::TestOutcome run_navmesh_pathfinding_lab() {
    navmesh_pathfinding_lab::init();
    auto s = navmesh_pathfinding_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    navmesh_pathfinding_lab::shutdown();
    return testing::ok();
}

const int _reg_navmesh_pathfinding_lab = testing::register_test(
    "navmesh_pathfinding_lab",
    "302_navmesh_pathfinding_lab: stub status check",
    &run_navmesh_pathfinding_lab);

}
