#include "test_runner.hpp"
#include "../modules/1257_camera_calibrator/camera_calibrator.hpp"

namespace {

testing::TestOutcome run_camera_calibrator() {
    camera_calibrator::init();
    auto s = camera_calibrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    camera_calibrator::shutdown();
    return testing::ok();
}

const int _reg_camera_calibrator = testing::register_test(
    "camera_calibrator",
    "1257_camera_calibrator: stub status check",
    &run_camera_calibrator);

}
