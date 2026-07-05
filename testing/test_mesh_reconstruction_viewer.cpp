// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/326_mesh_reconstruction_viewer/mesh_reconstruction_viewer.hpp"

namespace {

testing::TestOutcome run_mesh_reconstruction_viewer() {
    mesh_reconstruction_viewer::init();
    auto s = mesh_reconstruction_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mesh_reconstruction_viewer::shutdown();
    return testing::ok();
}

const int _reg_mesh_reconstruction_viewer = testing::register_test(
    "mesh_reconstruction_viewer",
    "326_mesh_reconstruction_viewer: stub status check",
    &run_mesh_reconstruction_viewer);

}
