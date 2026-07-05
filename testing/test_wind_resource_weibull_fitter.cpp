#include "test_runner.hpp"
#include "../modules/1285_wind_resource_weibull_fitter/wind_resource_weibull_fitter.hpp"

namespace {

testing::TestOutcome run_wind_resource_weibull_fitter() {
    wind_resource_weibull_fitter::init();
    auto s = wind_resource_weibull_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wind_resource_weibull_fitter::shutdown();
    return testing::ok();
}

const int _reg_wind_resource_weibull_fitter = testing::register_test(
    "wind_resource_weibull_fitter",
    "1285_wind_resource_weibull_fitter: stub status check",
    &run_wind_resource_weibull_fitter);

}
