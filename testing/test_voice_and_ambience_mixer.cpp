#include "test_runner.hpp"
#include "../modules/1962_voice_and_ambience_mixer/voice_and_ambience_mixer.hpp"

namespace {

testing::TestOutcome run_voice_and_ambience_mixer() {
    voice_and_ambience_mixer::init();
    auto s = voice_and_ambience_mixer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    voice_and_ambience_mixer::shutdown();
    return testing::ok();
}

const int _reg_voice_and_ambience_mixer = testing::register_test(
    "voice_and_ambience_mixer",
    "1962_voice_and_ambience_mixer: stub status check",
    &run_voice_and_ambience_mixer);

}
