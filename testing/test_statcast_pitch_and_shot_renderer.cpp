#include "test_runner.hpp"
#include "../modules/2091_statcast_pitch_and_shot_renderer/statcast_pitch_and_shot_renderer.hpp"

namespace {

testing::TestOutcome run_statcast_pitch_and_shot_renderer() {
    statcast_pitch_and_shot_renderer::init();
    auto s = statcast_pitch_and_shot_renderer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    statcast_pitch_and_shot_renderer::shutdown();
    return testing::ok();
}

const int _reg_statcast_pitch_and_shot_renderer = testing::register_test(
    "statcast_pitch_and_shot_renderer",
    "2091_statcast_pitch_and_shot_renderer: stub status check",
    &run_statcast_pitch_and_shot_renderer);

}
