// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2179_validated_instrument_battery/validated_instrument_battery.hpp"

namespace {

testing::TestOutcome run_validated_instrument_battery() {
    validated_instrument_battery::init();
    auto s = validated_instrument_battery::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    validated_instrument_battery::shutdown();
    return testing::ok();
}

const int _reg_validated_instrument_battery = testing::register_test(
    "validated_instrument_battery",
    "2179_validated_instrument_battery: stub status check",
    &run_validated_instrument_battery);

}
