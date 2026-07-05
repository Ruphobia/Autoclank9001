#include "test_runner.hpp"
#include "../modules/908_circuit_physics_sim/circuit_physics_sim.hpp"

namespace {

testing::TestOutcome run_circuit_physics_sim() {
    circuit_physics_sim::init();
    auto s = circuit_physics_sim::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    circuit_physics_sim::shutdown();
    return testing::ok();
}

const int _reg_circuit_physics_sim = testing::register_test(
    "circuit_physics_sim",
    "908_circuit_physics_sim: stub status check",
    &run_circuit_physics_sim);

}
