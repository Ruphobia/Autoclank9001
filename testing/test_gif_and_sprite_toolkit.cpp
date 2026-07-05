// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1652_gif_and_sprite_toolkit/gif_and_sprite_toolkit.hpp"

namespace {

testing::TestOutcome run_gif_and_sprite_toolkit() {
    gif_and_sprite_toolkit::init();
    auto s = gif_and_sprite_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gif_and_sprite_toolkit::shutdown();
    return testing::ok();
}

const int _reg_gif_and_sprite_toolkit = testing::register_test(
    "gif_and_sprite_toolkit",
    "1652_gif_and_sprite_toolkit: stub status check",
    &run_gif_and_sprite_toolkit);

}
