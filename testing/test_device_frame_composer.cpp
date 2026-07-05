// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/159_device_frame_composer/device_frame_composer.hpp"

namespace {

testing::TestOutcome run_device_frame_composer() {
    device_frame_composer::init();
    auto s = device_frame_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    device_frame_composer::shutdown();
    return testing::ok();
}

const int _reg_device_frame_composer = testing::register_test(
    "device_frame_composer",
    "159_device_frame_composer: stub status check",
    &run_device_frame_composer);

}
