// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/193_bootloader_builder/bootloader_builder.hpp"

namespace {

testing::TestOutcome run_bootloader_builder() {
    bootloader_builder::init();
    auto s = bootloader_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bootloader_builder::shutdown();
    return testing::ok();
}

const int _reg_bootloader_builder = testing::register_test(
    "bootloader_builder",
    "193_bootloader_builder: stub status check",
    &run_bootloader_builder);

}
