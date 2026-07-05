// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1506_wildlife_telemetry_analyzer/wildlife_telemetry_analyzer.hpp"

namespace {

testing::TestOutcome run_wildlife_telemetry_analyzer() {
    wildlife_telemetry_analyzer::init();
    auto s = wildlife_telemetry_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wildlife_telemetry_analyzer::shutdown();
    return testing::ok();
}

const int _reg_wildlife_telemetry_analyzer = testing::register_test(
    "wildlife_telemetry_analyzer",
    "1506_wildlife_telemetry_analyzer: stub status check",
    &run_wildlife_telemetry_analyzer);

}
