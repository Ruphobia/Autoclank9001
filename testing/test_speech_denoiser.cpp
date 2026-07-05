#include "test_runner.hpp"
#include "../modules/830_speech_denoiser/speech_denoiser.hpp"

namespace {

testing::TestOutcome run_speech_denoiser() {
    speech_denoiser::init();
    auto s = speech_denoiser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    speech_denoiser::shutdown();
    return testing::ok();
}

const int _reg_speech_denoiser = testing::register_test(
    "speech_denoiser",
    "830_speech_denoiser: stub status check",
    &run_speech_denoiser);

}
