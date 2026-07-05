#include "test_runner.hpp"
#include "../modules/1502_allometric_dose_calculator/allometric_dose_calculator.hpp"

namespace {

testing::TestOutcome run_allometric_dose_calculator() {
    allometric_dose_calculator::init();
    auto s = allometric_dose_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    allometric_dose_calculator::shutdown();
    return testing::ok();
}

const int _reg_allometric_dose_calculator = testing::register_test(
    "allometric_dose_calculator",
    "1502_allometric_dose_calculator: stub status check",
    &run_allometric_dose_calculator);

}
