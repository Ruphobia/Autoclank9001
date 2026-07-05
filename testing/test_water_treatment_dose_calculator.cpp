#include "test_runner.hpp"
#include "../modules/2114_water_treatment_dose_calculator/water_treatment_dose_calculator.hpp"

namespace {

testing::TestOutcome run_water_treatment_dose_calculator() {
    water_treatment_dose_calculator::init();
    auto s = water_treatment_dose_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    water_treatment_dose_calculator::shutdown();
    return testing::ok();
}

const int _reg_water_treatment_dose_calculator = testing::register_test(
    "water_treatment_dose_calculator",
    "2114_water_treatment_dose_calculator: stub status check",
    &run_water_treatment_dose_calculator);

}
