#include "test_runner.hpp"
#include "../modules/2051_ballistics_and_arrow_calculator/ballistics_and_arrow_calculator.hpp"

namespace {

testing::TestOutcome run_ballistics_and_arrow_calculator() {
    ballistics_and_arrow_calculator::init();
    auto s = ballistics_and_arrow_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ballistics_and_arrow_calculator::shutdown();
    return testing::ok();
}

const int _reg_ballistics_and_arrow_calculator = testing::register_test(
    "ballistics_and_arrow_calculator",
    "2051_ballistics_and_arrow_calculator: stub status check",
    &run_ballistics_and_arrow_calculator);

}
