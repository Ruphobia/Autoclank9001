#include "test_runner.hpp"
#include "../modules/2068_anchor_force_calculator/anchor_force_calculator.hpp"

namespace {

testing::TestOutcome run_anchor_force_calculator() {
    anchor_force_calculator::init();
    auto s = anchor_force_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anchor_force_calculator::shutdown();
    return testing::ok();
}

const int _reg_anchor_force_calculator = testing::register_test(
    "anchor_force_calculator",
    "2068_anchor_force_calculator: stub status check",
    &run_anchor_force_calculator);

}
