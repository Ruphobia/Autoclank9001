// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/222_field_telemetry_browser/field_telemetry_browser.hpp"

namespace {

testing::TestOutcome run_field_telemetry_browser() {
    field_telemetry_browser::init();
    auto s = field_telemetry_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    field_telemetry_browser::shutdown();
    return testing::ok();
}

const int _reg_field_telemetry_browser = testing::register_test(
    "field_telemetry_browser",
    "222_field_telemetry_browser: stub status check",
    &run_field_telemetry_browser);

}
