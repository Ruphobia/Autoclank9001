// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/407_rom_patch_studio/rom_patch_studio.hpp"

namespace {

testing::TestOutcome run_rom_patch_studio() {
    rom_patch_studio::init();
    auto s = rom_patch_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rom_patch_studio::shutdown();
    return testing::ok();
}

const int _reg_rom_patch_studio = testing::register_test(
    "rom_patch_studio",
    "407_rom_patch_studio: stub status check",
    &run_rom_patch_studio);

}
