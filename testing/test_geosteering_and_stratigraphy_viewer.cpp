#include "test_runner.hpp"
#include "../modules/1088_geosteering_and_stratigraphy_viewer/geosteering_and_stratigraphy_viewer.hpp"

namespace {

testing::TestOutcome run_geosteering_and_stratigraphy_viewer() {
    geosteering_and_stratigraphy_viewer::init();
    auto s = geosteering_and_stratigraphy_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geosteering_and_stratigraphy_viewer::shutdown();
    return testing::ok();
}

const int _reg_geosteering_and_stratigraphy_viewer = testing::register_test(
    "geosteering_and_stratigraphy_viewer",
    "1088_geosteering_and_stratigraphy_viewer: stub status check",
    &run_geosteering_and_stratigraphy_viewer);

}
