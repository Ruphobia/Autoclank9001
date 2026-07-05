#include "test_runner.hpp"
#include "../modules/1552_wave_spectrum_analyzer/wave_spectrum_analyzer.hpp"

namespace {

testing::TestOutcome run_wave_spectrum_analyzer() {
    wave_spectrum_analyzer::init();
    auto s = wave_spectrum_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wave_spectrum_analyzer::shutdown();
    return testing::ok();
}

const int _reg_wave_spectrum_analyzer = testing::register_test(
    "wave_spectrum_analyzer",
    "1552_wave_spectrum_analyzer: stub status check",
    &run_wave_spectrum_analyzer);

}
