#include "test_runner.hpp"
#include "../modules/1208_spring_and_motion_ratio_calculator/spring_and_motion_ratio_calculator.hpp"

namespace {

testing::TestOutcome run_spring_and_motion_ratio_calculator() {
    spring_and_motion_ratio_calculator::init();
    auto s = spring_and_motion_ratio_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spring_and_motion_ratio_calculator::shutdown();
    return testing::ok();
}

const int _reg_spring_and_motion_ratio_calculator = testing::register_test(
    "spring_and_motion_ratio_calculator",
    "1208_spring_and_motion_ratio_calculator: stub status check",
    &run_spring_and_motion_ratio_calculator);

}
