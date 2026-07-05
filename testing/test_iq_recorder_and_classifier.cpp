// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1193_iq_recorder_and_classifier/iq_recorder_and_classifier.hpp"

namespace {

testing::TestOutcome run_iq_recorder_and_classifier() {
    iq_recorder_and_classifier::init();
    auto s = iq_recorder_and_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    iq_recorder_and_classifier::shutdown();
    return testing::ok();
}

const int _reg_iq_recorder_and_classifier = testing::register_test(
    "iq_recorder_and_classifier",
    "1193_iq_recorder_and_classifier: stub status check",
    &run_iq_recorder_and_classifier);

}
