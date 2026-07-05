#include "test_runner.hpp"
#include "../modules/1039_wind_load_calculator/wind_load_calculator.hpp"

namespace {

testing::TestOutcome run_wind_load_calculator() {
    wind_load_calculator::init();
    auto s = wind_load_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wind_load_calculator::shutdown();
    return testing::ok();
}

const int _reg_wind_load_calculator = testing::register_test(
    "wind_load_calculator",
    "1039_wind_load_calculator: stub status check",
    &run_wind_load_calculator);

}
