#include "test_runner.hpp"
#include "../modules/2172_mood_log_with_valence_arousal_grid/mood_log_with_valence_arousal_grid.hpp"

namespace {

testing::TestOutcome run_mood_log_with_valence_arousal_grid() {
    mood_log_with_valence_arousal_grid::init();
    auto s = mood_log_with_valence_arousal_grid::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mood_log_with_valence_arousal_grid::shutdown();
    return testing::ok();
}

const int _reg_mood_log_with_valence_arousal_grid = testing::register_test(
    "mood_log_with_valence_arousal_grid",
    "2172_mood_log_with_valence_arousal_grid: stub status check",
    &run_mood_log_with_valence_arousal_grid);

}
