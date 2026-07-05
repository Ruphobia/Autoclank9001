// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/286_tilemap_composer/tilemap_composer.hpp"

namespace {

testing::TestOutcome run_tilemap_composer() {
    tilemap_composer::init();
    auto s = tilemap_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tilemap_composer::shutdown();
    return testing::ok();
}

const int _reg_tilemap_composer = testing::register_test(
    "tilemap_composer",
    "286_tilemap_composer: stub status check",
    &run_tilemap_composer);

}
