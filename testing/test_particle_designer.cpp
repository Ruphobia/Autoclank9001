#include "test_runner.hpp"
#include "../modules/287_particle_designer/particle_designer.hpp"

namespace {

testing::TestOutcome run_particle_designer() {
    particle_designer::init();
    auto s = particle_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    particle_designer::shutdown();
    return testing::ok();
}

const int _reg_particle_designer = testing::register_test(
    "particle_designer",
    "287_particle_designer: stub status check",
    &run_particle_designer);

}
