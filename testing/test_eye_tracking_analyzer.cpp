// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1435_eye_tracking_analyzer/eye_tracking_analyzer.hpp"

namespace {

testing::TestOutcome run_eye_tracking_analyzer() {
    eye_tracking_analyzer::init();
    auto s = eye_tracking_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    eye_tracking_analyzer::shutdown();
    return testing::ok();
}

const int _reg_eye_tracking_analyzer = testing::register_test(
    "eye_tracking_analyzer",
    "1435_eye_tracking_analyzer: stub status check",
    &run_eye_tracking_analyzer);

}
