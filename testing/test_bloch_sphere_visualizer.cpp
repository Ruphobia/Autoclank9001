#include "test_runner.hpp"
#include "../modules/331_bloch_sphere_visualizer/bloch_sphere_visualizer.hpp"

namespace {

testing::TestOutcome run_bloch_sphere_visualizer() {
    bloch_sphere_visualizer::init();
    auto s = bloch_sphere_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bloch_sphere_visualizer::shutdown();
    return testing::ok();
}

const int _reg_bloch_sphere_visualizer = testing::register_test(
    "bloch_sphere_visualizer",
    "331_bloch_sphere_visualizer: stub status check",
    &run_bloch_sphere_visualizer);

}
