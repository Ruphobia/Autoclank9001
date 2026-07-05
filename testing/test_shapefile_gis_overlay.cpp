// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1439_shapefile_gis_overlay/shapefile_gis_overlay.hpp"

namespace {

testing::TestOutcome run_shapefile_gis_overlay() {
    shapefile_gis_overlay::init();
    auto s = shapefile_gis_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    shapefile_gis_overlay::shutdown();
    return testing::ok();
}

const int _reg_shapefile_gis_overlay = testing::register_test(
    "shapefile_gis_overlay",
    "1439_shapefile_gis_overlay: stub status check",
    &run_shapefile_gis_overlay);

}
