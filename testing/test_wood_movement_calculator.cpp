#include "test_runner.hpp"
#include "../modules/980_wood_movement_calculator/wood_movement_calculator.hpp"

namespace {

testing::TestOutcome run_wood_movement_calculator() {
    wood_movement_calculator::init();
    auto s = wood_movement_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wood_movement_calculator::shutdown();
    return testing::ok();
}

const int _reg_wood_movement_calculator = testing::register_test(
    "wood_movement_calculator",
    "980_wood_movement_calculator: stub status check",
    &run_wood_movement_calculator);

}
