// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1501_spectrogram_annotator/spectrogram_annotator.hpp"

namespace {

testing::TestOutcome run_spectrogram_annotator() {
    spectrogram_annotator::init();
    auto s = spectrogram_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrogram_annotator::shutdown();
    return testing::ok();
}

const int _reg_spectrogram_annotator = testing::register_test(
    "spectrogram_annotator",
    "1501_spectrogram_annotator: stub status check",
    &run_spectrogram_annotator);

}
