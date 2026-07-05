// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/714_meter_and_prosody_scanner/meter_and_prosody_scanner.hpp"

namespace {

testing::TestOutcome run_meter_and_prosody_scanner() {
    meter_and_prosody_scanner::init();
    auto s = meter_and_prosody_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meter_and_prosody_scanner::shutdown();
    return testing::ok();
}

const int _reg_meter_and_prosody_scanner = testing::register_test(
    "meter_and_prosody_scanner",
    "714_meter_and_prosody_scanner: stub status check",
    &run_meter_and_prosody_scanner);

}
