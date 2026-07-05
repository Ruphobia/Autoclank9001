#include "test_runner.hpp"
#include "../modules/1784_nutrition_calculator/nutrition_calculator.hpp"

namespace {

testing::TestOutcome run_nutrition_calculator() {
    nutrition_calculator::init();
    auto s = nutrition_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nutrition_calculator::shutdown();
    return testing::ok();
}

const int _reg_nutrition_calculator = testing::register_test(
    "nutrition_calculator",
    "1784_nutrition_calculator: stub status check",
    &run_nutrition_calculator);

}
