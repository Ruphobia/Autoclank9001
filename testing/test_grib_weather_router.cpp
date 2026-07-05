#include "test_runner.hpp"
#include "../modules/2096_grib_weather_router/grib_weather_router.hpp"

namespace {

testing::TestOutcome run_grib_weather_router() {
    grib_weather_router::init();
    auto s = grib_weather_router::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grib_weather_router::shutdown();
    return testing::ok();
}

const int _reg_grib_weather_router = testing::register_test(
    "grib_weather_router",
    "2096_grib_weather_router: stub status check",
    &run_grib_weather_router);

}
