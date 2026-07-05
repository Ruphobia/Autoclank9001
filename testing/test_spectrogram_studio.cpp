#include "test_runner.hpp"
#include "../modules/795_spectrogram_studio/spectrogram_studio.hpp"

namespace {

testing::TestOutcome run_spectrogram_studio() {
    spectrogram_studio::init();
    auto s = spectrogram_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrogram_studio::shutdown();
    return testing::ok();
}

const int _reg_spectrogram_studio = testing::register_test(
    "spectrogram_studio",
    "795_spectrogram_studio: stub status check",
    &run_spectrogram_studio);

}
