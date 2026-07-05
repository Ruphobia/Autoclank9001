// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2093_raster_chart_georeferencer/raster_chart_georeferencer.hpp"

namespace {

testing::TestOutcome run_raster_chart_georeferencer() {
    raster_chart_georeferencer::init();
    auto s = raster_chart_georeferencer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    raster_chart_georeferencer::shutdown();
    return testing::ok();
}

const int _reg_raster_chart_georeferencer = testing::register_test(
    "raster_chart_georeferencer",
    "2093_raster_chart_georeferencer: stub status check",
    &run_raster_chart_georeferencer);

}
