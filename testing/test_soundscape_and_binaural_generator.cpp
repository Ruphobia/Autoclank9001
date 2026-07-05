#include "test_runner.hpp"
#include "../modules/2189_soundscape_and_binaural_generator/soundscape_and_binaural_generator.hpp"

namespace {

testing::TestOutcome run_soundscape_and_binaural_generator() {
    soundscape_and_binaural_generator::init();
    auto s = soundscape_and_binaural_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    soundscape_and_binaural_generator::shutdown();
    return testing::ok();
}

const int _reg_soundscape_and_binaural_generator = testing::register_test(
    "soundscape_and_binaural_generator",
    "2189_soundscape_and_binaural_generator: stub status check",
    &run_soundscape_and_binaural_generator);

}
