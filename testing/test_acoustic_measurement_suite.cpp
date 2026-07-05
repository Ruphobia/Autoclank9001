// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1919_acoustic_measurement_suite/acoustic_measurement_suite.hpp"

namespace {

testing::TestOutcome run_acoustic_measurement_suite() {
    acoustic_measurement_suite::init();
    auto s = acoustic_measurement_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    acoustic_measurement_suite::shutdown();
    return testing::ok();
}

const int _reg_acoustic_measurement_suite = testing::register_test(
    "acoustic_measurement_suite",
    "1919_acoustic_measurement_suite: stub status check",
    &run_acoustic_measurement_suite);

}
