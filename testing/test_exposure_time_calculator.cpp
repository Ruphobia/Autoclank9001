#include "test_runner.hpp"
#include "../modules/1498_exposure_time_calculator/exposure_time_calculator.hpp"

namespace {

testing::TestOutcome run_exposure_time_calculator() {
    exposure_time_calculator::init();
    auto s = exposure_time_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    exposure_time_calculator::shutdown();
    return testing::ok();
}

const int _reg_exposure_time_calculator = testing::register_test(
    "exposure_time_calculator",
    "1498_exposure_time_calculator: stub status check",
    &run_exposure_time_calculator);

}
