// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1620_advanced_raster_image_editor_photoshop_gimp_class/advanced_raster_image_editor_photoshop_gimp_class.hpp"

namespace {

testing::TestOutcome run_advanced_raster_image_editor_photoshop_gimp_class() {
    advanced_raster_image_editor_photoshop_gimp_class::init();
    auto s = advanced_raster_image_editor_photoshop_gimp_class::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    advanced_raster_image_editor_photoshop_gimp_class::shutdown();
    return testing::ok();
}

const int _reg_advanced_raster_image_editor_photoshop_gimp_class = testing::register_test(
    "advanced_raster_image_editor_photoshop_gimp_class",
    "1620_advanced_raster_image_editor_photoshop_gimp_class: stub status check",
    &run_advanced_raster_image_editor_photoshop_gimp_class);

}
