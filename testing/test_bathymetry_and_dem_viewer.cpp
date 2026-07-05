#include "test_runner.hpp"
#include "../modules/1124_bathymetry_and_dem_viewer/bathymetry_and_dem_viewer.hpp"

namespace {

testing::TestOutcome run_bathymetry_and_dem_viewer() {
    bathymetry_and_dem_viewer::init();
    auto s = bathymetry_and_dem_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bathymetry_and_dem_viewer::shutdown();
    return testing::ok();
}

const int _reg_bathymetry_and_dem_viewer = testing::register_test(
    "bathymetry_and_dem_viewer",
    "1124_bathymetry_and_dem_viewer: stub status check",
    &run_bathymetry_and_dem_viewer);

}
