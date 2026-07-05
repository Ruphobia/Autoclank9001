// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/367_firmware_and_binary_walker/firmware_and_binary_walker.hpp"

namespace {

testing::TestOutcome run_firmware_and_binary_walker() {
    firmware_and_binary_walker::init();
    auto s = firmware_and_binary_walker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    firmware_and_binary_walker::shutdown();
    return testing::ok();
}

const int _reg_firmware_and_binary_walker = testing::register_test(
    "firmware_and_binary_walker",
    "367_firmware_and_binary_walker: stub status check",
    &run_firmware_and_binary_walker);

}
