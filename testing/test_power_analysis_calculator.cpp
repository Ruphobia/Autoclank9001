#include "test_runner.hpp"
#include "../modules/1422_power_analysis_calculator/power_analysis_calculator.hpp"

namespace {

testing::TestOutcome run_power_analysis_calculator() {
    power_analysis_calculator::init();
    auto s = power_analysis_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    power_analysis_calculator::shutdown();
    return testing::ok();
}

const int _reg_power_analysis_calculator = testing::register_test(
    "power_analysis_calculator",
    "1422_power_analysis_calculator: stub status check",
    &run_power_analysis_calculator);

}
