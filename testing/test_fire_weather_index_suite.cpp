#include "test_runner.hpp"
#include "../modules/1469_fire_weather_index_suite/fire_weather_index_suite.hpp"

namespace {

testing::TestOutcome run_fire_weather_index_suite() {
    fire_weather_index_suite::init();
    auto s = fire_weather_index_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fire_weather_index_suite::shutdown();
    return testing::ok();
}

const int _reg_fire_weather_index_suite = testing::register_test(
    "fire_weather_index_suite",
    "1469_fire_weather_index_suite: stub status check",
    &run_fire_weather_index_suite);

}
