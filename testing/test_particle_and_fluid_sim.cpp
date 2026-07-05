#include "test_runner.hpp"
#include "../modules/1687_particle_and_fluid_sim/particle_and_fluid_sim.hpp"

namespace {

testing::TestOutcome run_particle_and_fluid_sim() {
    particle_and_fluid_sim::init();
    auto s = particle_and_fluid_sim::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    particle_and_fluid_sim::shutdown();
    return testing::ok();
}

const int _reg_particle_and_fluid_sim = testing::register_test(
    "particle_and_fluid_sim",
    "1687_particle_and_fluid_sim: stub status check",
    &run_particle_and_fluid_sim);

}
