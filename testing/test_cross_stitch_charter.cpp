// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1770_cross_stitch_charter/cross_stitch_charter.hpp"

namespace {

testing::TestOutcome run_cross_stitch_charter() {
    cross_stitch_charter::init();
    auto s = cross_stitch_charter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cross_stitch_charter::shutdown();
    return testing::ok();
}

const int _reg_cross_stitch_charter = testing::register_test(
    "cross_stitch_charter",
    "1770_cross_stitch_charter: stub status check",
    &run_cross_stitch_charter);

}
