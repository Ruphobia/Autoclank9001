// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1918_speaker_coverage_predictor/speaker_coverage_predictor.hpp"

namespace {

testing::TestOutcome run_speaker_coverage_predictor() {
    speaker_coverage_predictor::init();
    auto s = speaker_coverage_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    speaker_coverage_predictor::shutdown();
    return testing::ok();
}

const int _reg_speaker_coverage_predictor = testing::register_test(
    "speaker_coverage_predictor",
    "1918_speaker_coverage_predictor: stub status check",
    &run_speaker_coverage_predictor);

}
