#include "test_runner.hpp"
#include "../modules/818_transcription_studio/transcription_studio.hpp"

namespace {

testing::TestOutcome run_transcription_studio() {
    transcription_studio::init();
    auto s = transcription_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transcription_studio::shutdown();
    return testing::ok();
}

const int _reg_transcription_studio = testing::register_test(
    "transcription_studio",
    "818_transcription_studio: stub status check",
    &run_transcription_studio);

}
