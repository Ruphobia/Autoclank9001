#include "test_runner.hpp"
#include "../modules/2060_slope_angle_aspect_map/slope_angle_aspect_map.hpp"

namespace {

testing::TestOutcome run_slope_angle_aspect_map() {
    slope_angle_aspect_map::init();
    auto s = slope_angle_aspect_map::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    slope_angle_aspect_map::shutdown();
    return testing::ok();
}

const int _reg_slope_angle_aspect_map = testing::register_test(
    "slope_angle_aspect_map",
    "2060_slope_angle_aspect_map: stub status check",
    &run_slope_angle_aspect_map);

}
