#include "test_runner.hpp"
#include "../modules/1526_mesh_reconstruction_lab/mesh_reconstruction_lab.hpp"

namespace {

testing::TestOutcome run_mesh_reconstruction_lab() {
    mesh_reconstruction_lab::init();
    auto s = mesh_reconstruction_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mesh_reconstruction_lab::shutdown();
    return testing::ok();
}

const int _reg_mesh_reconstruction_lab = testing::register_test(
    "mesh_reconstruction_lab",
    "1526_mesh_reconstruction_lab: stub status check",
    &run_mesh_reconstruction_lab);

}
