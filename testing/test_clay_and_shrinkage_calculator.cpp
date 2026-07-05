#include "test_runner.hpp"
#include "../modules/1875_clay_and_shrinkage_calculator/clay_and_shrinkage_calculator.hpp"

namespace {

testing::TestOutcome run_clay_and_shrinkage_calculator() {
    clay_and_shrinkage_calculator::init();
    auto s = clay_and_shrinkage_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    clay_and_shrinkage_calculator::shutdown();
    return testing::ok();
}

const int _reg_clay_and_shrinkage_calculator = testing::register_test(
    "clay_and_shrinkage_calculator",
    "1875_clay_and_shrinkage_calculator: stub status check",
    &run_clay_and_shrinkage_calculator);

}
