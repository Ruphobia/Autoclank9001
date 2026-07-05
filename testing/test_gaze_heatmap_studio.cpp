#include "test_runner.hpp"
#include "../modules/324_gaze_heatmap_studio/gaze_heatmap_studio.hpp"

namespace {

testing::TestOutcome run_gaze_heatmap_studio() {
    gaze_heatmap_studio::init();
    auto s = gaze_heatmap_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gaze_heatmap_studio::shutdown();
    return testing::ok();
}

const int _reg_gaze_heatmap_studio = testing::register_test(
    "gaze_heatmap_studio",
    "324_gaze_heatmap_studio: stub status check",
    &run_gaze_heatmap_studio);

}
