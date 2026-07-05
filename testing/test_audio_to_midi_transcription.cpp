#include "test_runner.hpp"
#include "../modules/1742_audio_to_midi_transcription/audio_to_midi_transcription.hpp"

namespace {

testing::TestOutcome run_audio_to_midi_transcription() {
    audio_to_midi_transcription::init();
    auto s = audio_to_midi_transcription::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audio_to_midi_transcription::shutdown();
    return testing::ok();
}

const int _reg_audio_to_midi_transcription = testing::register_test(
    "audio_to_midi_transcription",
    "1742_audio_to_midi_transcription: stub status check",
    &run_audio_to_midi_transcription);

}
