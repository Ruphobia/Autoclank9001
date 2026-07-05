// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1184_digital_mode_bridge/digital_mode_bridge.hpp"

namespace {

testing::TestOutcome run_digital_mode_bridge() {
    digital_mode_bridge::init();
    auto s = digital_mode_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    digital_mode_bridge::shutdown();
    return testing::ok();
}

const int _reg_digital_mode_bridge = testing::register_test(
    "digital_mode_bridge",
    "1184_digital_mode_bridge: stub status check",
    &run_digital_mode_bridge);

}
