#include "test_runner.hpp"
#include "../modules/1390_eeg_meg_analyzer/eeg_meg_analyzer.hpp"

namespace {

testing::TestOutcome run_eeg_meg_analyzer() {
    eeg_meg_analyzer::init();
    auto s = eeg_meg_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    eeg_meg_analyzer::shutdown();
    return testing::ok();
}

const int _reg_eeg_meg_analyzer = testing::register_test(
    "eeg_meg_analyzer",
    "1390_eeg_meg_analyzer: stub status check",
    &run_eeg_meg_analyzer);

}
