// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/768_pantheon_cross_reference_atlas/pantheon_cross_reference_atlas.hpp"

namespace {

testing::TestOutcome run_pantheon_cross_reference_atlas() {
    pantheon_cross_reference_atlas::init();
    auto s = pantheon_cross_reference_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pantheon_cross_reference_atlas::shutdown();
    return testing::ok();
}

const int _reg_pantheon_cross_reference_atlas = testing::register_test(
    "pantheon_cross_reference_atlas",
    "768_pantheon_cross_reference_atlas: stub status check",
    &run_pantheon_cross_reference_atlas);

}
