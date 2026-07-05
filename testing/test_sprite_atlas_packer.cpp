// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/298_sprite_atlas_packer/sprite_atlas_packer.hpp"

namespace {

testing::TestOutcome run_sprite_atlas_packer() {
    sprite_atlas_packer::init();
    auto s = sprite_atlas_packer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sprite_atlas_packer::shutdown();
    return testing::ok();
}

const int _reg_sprite_atlas_packer = testing::register_test(
    "sprite_atlas_packer",
    "298_sprite_atlas_packer: stub status check",
    &run_sprite_atlas_packer);

}
