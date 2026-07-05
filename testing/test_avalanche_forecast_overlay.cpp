#include "test_runner.hpp"
#include "../modules/2059_avalanche_forecast_overlay/avalanche_forecast_overlay.hpp"

namespace {

testing::TestOutcome run_avalanche_forecast_overlay() {
    avalanche_forecast_overlay::init();
    auto s = avalanche_forecast_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    avalanche_forecast_overlay::shutdown();
    return testing::ok();
}

const int _reg_avalanche_forecast_overlay = testing::register_test(
    "avalanche_forecast_overlay",
    "2059_avalanche_forecast_overlay: stub status check",
    &run_avalanche_forecast_overlay);

}
