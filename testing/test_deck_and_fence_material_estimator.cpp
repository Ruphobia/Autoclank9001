// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2035_deck_and_fence_material_estimator/deck_and_fence_material_estimator.hpp"

namespace {

testing::TestOutcome run_deck_and_fence_material_estimator() {
    deck_and_fence_material_estimator::init();
    auto s = deck_and_fence_material_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    deck_and_fence_material_estimator::shutdown();
    return testing::ok();
}

const int _reg_deck_and_fence_material_estimator = testing::register_test(
    "deck_and_fence_material_estimator",
    "2035_deck_and_fence_material_estimator: stub status check",
    &run_deck_and_fence_material_estimator);

}
