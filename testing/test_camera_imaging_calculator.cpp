#include "test_runner.hpp"
#include "../modules/883_camera_imaging_calculator/camera_imaging_calculator.hpp"

namespace {

testing::TestOutcome run_camera_imaging_calculator() {
    camera_imaging_calculator::init();
    auto s = camera_imaging_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    camera_imaging_calculator::shutdown();
    return testing::ok();
}

const int _reg_camera_imaging_calculator = testing::register_test(
    "camera_imaging_calculator",
    "883_camera_imaging_calculator: stub status check",
    &run_camera_imaging_calculator);

}
