#include "test_runner.hpp"
#include "../modules/1726_spectrogram_analyzer/spectrogram_analyzer.hpp"

namespace {

testing::TestOutcome run_spectrogram_analyzer() {
    spectrogram_analyzer::init();
    auto s = spectrogram_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrogram_analyzer::shutdown();
    return testing::ok();
}

const int _reg_spectrogram_analyzer = testing::register_test(
    "spectrogram_analyzer",
    "1726_spectrogram_analyzer: stub status check",
    &run_spectrogram_analyzer);

}
