#include "test_runner.hpp"
#include "../modules/2165_snore_and_apnea_audio_analyzer/snore_and_apnea_audio_analyzer.hpp"

namespace {

testing::TestOutcome run_snore_and_apnea_audio_analyzer() {
    snore_and_apnea_audio_analyzer::init();
    auto s = snore_and_apnea_audio_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    snore_and_apnea_audio_analyzer::shutdown();
    return testing::ok();
}

const int _reg_snore_and_apnea_audio_analyzer = testing::register_test(
    "snore_and_apnea_audio_analyzer",
    "2165_snore_and_apnea_audio_analyzer: stub status check",
    &run_snore_and_apnea_audio_analyzer);

}
