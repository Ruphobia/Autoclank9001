#include "test_runner.hpp"
#include "../modules/439_roadmap_visualizer/roadmap_visualizer.hpp"

namespace {

testing::TestOutcome run_roadmap_visualizer() {
    roadmap_visualizer::init();
    auto s = roadmap_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    roadmap_visualizer::shutdown();
    return testing::ok();
}

const int _reg_roadmap_visualizer = testing::register_test(
    "roadmap_visualizer",
    "439_roadmap_visualizer: stub status check",
    &run_roadmap_visualizer);

}
