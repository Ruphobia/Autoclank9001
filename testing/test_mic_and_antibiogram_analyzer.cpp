#include "test_runner.hpp"
#include "../modules/1370_mic_and_antibiogram_analyzer/mic_and_antibiogram_analyzer.hpp"

namespace {

testing::TestOutcome run_mic_and_antibiogram_analyzer() {
    mic_and_antibiogram_analyzer::init();
    auto s = mic_and_antibiogram_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mic_and_antibiogram_analyzer::shutdown();
    return testing::ok();
}

const int _reg_mic_and_antibiogram_analyzer = testing::register_test(
    "mic_and_antibiogram_analyzer",
    "1370_mic_and_antibiogram_analyzer: stub status check",
    &run_mic_and_antibiogram_analyzer);

}
