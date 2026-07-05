// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/778_hero_journey_composer/hero_journey_composer.hpp"

namespace {

testing::TestOutcome run_hero_journey_composer() {
    hero_journey_composer::init();
    auto s = hero_journey_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hero_journey_composer::shutdown();
    return testing::ok();
}

const int _reg_hero_journey_composer = testing::register_test(
    "hero_journey_composer",
    "778_hero_journey_composer: stub status check",
    &run_hero_journey_composer);

}
