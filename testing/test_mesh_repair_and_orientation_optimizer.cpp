#include "test_runner.hpp"
#include "../modules/970_mesh_repair_and_orientation_optimizer/mesh_repair_and_orientation_optimizer.hpp"

namespace {

testing::TestOutcome run_mesh_repair_and_orientation_optimizer() {
    mesh_repair_and_orientation_optimizer::init();
    auto s = mesh_repair_and_orientation_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mesh_repair_and_orientation_optimizer::shutdown();
    return testing::ok();
}

const int _reg_mesh_repair_and_orientation_optimizer = testing::register_test(
    "mesh_repair_and_orientation_optimizer",
    "970_mesh_repair_and_orientation_optimizer: stub status check",
    &run_mesh_repair_and_orientation_optimizer);

}
