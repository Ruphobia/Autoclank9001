#include "test_runner.hpp"
#include "../modules/1611_spray_mix_calculator/spray_mix_calculator.hpp"

namespace {

testing::TestOutcome run_spray_mix_calculator() {
    spray_mix_calculator::init();
    auto s = spray_mix_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spray_mix_calculator::shutdown();
    return testing::ok();
}

const int _reg_spray_mix_calculator = testing::register_test(
    "spray_mix_calculator",
    "1611_spray_mix_calculator: stub status check",
    &run_spray_mix_calculator);

}
