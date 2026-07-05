// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1713_key_and_tempo_detector/key_and_tempo_detector.hpp"

namespace {

testing::TestOutcome run_key_and_tempo_detector() {
    key_and_tempo_detector::init();
    auto s = key_and_tempo_detector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    key_and_tempo_detector::shutdown();
    return testing::ok();
}

const int _reg_key_and_tempo_detector = testing::register_test(
    "key_and_tempo_detector",
    "1713_key_and_tempo_detector: stub status check",
    &run_key_and_tempo_detector);

}
