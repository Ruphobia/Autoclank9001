#include "test_runner.hpp"
#include "../modules/1262_sensor_stream_viewer/sensor_stream_viewer.hpp"

namespace {

testing::TestOutcome run_sensor_stream_viewer() {
    sensor_stream_viewer::init();
    auto s = sensor_stream_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sensor_stream_viewer::shutdown();
    return testing::ok();
}

const int _reg_sensor_stream_viewer = testing::register_test(
    "sensor_stream_viewer",
    "1262_sensor_stream_viewer: stub status check",
    &run_sensor_stream_viewer);

}
