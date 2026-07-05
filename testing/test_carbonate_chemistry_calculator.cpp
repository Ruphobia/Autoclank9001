#include "test_runner.hpp"
#include "../modules/1555_carbonate_chemistry_calculator/carbonate_chemistry_calculator.hpp"

namespace {

testing::TestOutcome run_carbonate_chemistry_calculator() {
    carbonate_chemistry_calculator::init();
    auto s = carbonate_chemistry_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    carbonate_chemistry_calculator::shutdown();
    return testing::ok();
}

const int _reg_carbonate_chemistry_calculator = testing::register_test(
    "carbonate_chemistry_calculator",
    "1555_carbonate_chemistry_calculator: stub status check",
    &run_carbonate_chemistry_calculator);

}
