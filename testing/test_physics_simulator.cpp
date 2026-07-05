#include "test_runner.hpp"
#include "../modules/1252_physics_simulator/physics_simulator.hpp"

namespace {

testing::TestOutcome run_physics_simulator() {
    physics_simulator::init();
    auto s = physics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    physics_simulator::shutdown();
    return testing::ok();
}

const int _reg_physics_simulator = testing::register_test(
    "physics_simulator",
    "1252_physics_simulator: stub status check",
    &run_physics_simulator);

}
