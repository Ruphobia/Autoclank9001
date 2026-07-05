#include "test_runner.hpp"
#include "../modules/1602_fertilizer_blend_calculator/fertilizer_blend_calculator.hpp"

namespace {

testing::TestOutcome run_fertilizer_blend_calculator() {
    fertilizer_blend_calculator::init();
    auto s = fertilizer_blend_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fertilizer_blend_calculator::shutdown();
    return testing::ok();
}

const int _reg_fertilizer_blend_calculator = testing::register_test(
    "fertilizer_blend_calculator",
    "1602_fertilizer_blend_calculator: stub status check",
    &run_fertilizer_blend_calculator);

}
