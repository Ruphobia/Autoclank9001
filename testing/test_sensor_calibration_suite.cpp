#include "test_runner.hpp"
#include "../modules/210_sensor_calibration_suite/sensor_calibration_suite.hpp"

namespace {

testing::TestOutcome run_sensor_calibration_suite() {
    sensor_calibration_suite::init();
    auto s = sensor_calibration_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sensor_calibration_suite::shutdown();
    return testing::ok();
}

const int _reg_sensor_calibration_suite = testing::register_test(
    "sensor_calibration_suite",
    "210_sensor_calibration_suite: stub status check",
    &run_sensor_calibration_suite);

}
