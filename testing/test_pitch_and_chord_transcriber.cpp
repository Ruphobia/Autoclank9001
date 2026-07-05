#include "test_runner.hpp"
#include "../modules/1714_pitch_and_chord_transcriber/pitch_and_chord_transcriber.hpp"

namespace {

testing::TestOutcome run_pitch_and_chord_transcriber() {
    pitch_and_chord_transcriber::init();
    auto s = pitch_and_chord_transcriber::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pitch_and_chord_transcriber::shutdown();
    return testing::ok();
}

const int _reg_pitch_and_chord_transcriber = testing::register_test(
    "pitch_and_chord_transcriber",
    "1714_pitch_and_chord_transcriber: stub status check",
    &run_pitch_and_chord_transcriber);

}
