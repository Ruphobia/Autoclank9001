// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/974_raster_engraving_halftoner/raster_engraving_halftoner.hpp"

namespace {

testing::TestOutcome run_raster_engraving_halftoner() {
    raster_engraving_halftoner::init();
    auto s = raster_engraving_halftoner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    raster_engraving_halftoner::shutdown();
    return testing::ok();
}

const int _reg_raster_engraving_halftoner = testing::register_test(
    "raster_engraving_halftoner",
    "974_raster_engraving_halftoner: stub status check",
    &run_raster_engraving_halftoner);

}
