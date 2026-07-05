#include "test_runner.hpp"
#include "../modules/1130_ground_track_visualizer/ground_track_visualizer.hpp"

namespace {

testing::TestOutcome run_ground_track_visualizer() {
    ground_track_visualizer::init();
    auto s = ground_track_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ground_track_visualizer::shutdown();
    return testing::ok();
}

const int _reg_ground_track_visualizer = testing::register_test(
    "ground_track_visualizer",
    "1130_ground_track_visualizer: stub status check",
    &run_ground_track_visualizer);

}
