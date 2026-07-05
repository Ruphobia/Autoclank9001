#include "test_runner.hpp"
#include "../modules/1787_equilibrium_cure_brine_calculator/equilibrium_cure_brine_calculator.hpp"

namespace {

testing::TestOutcome run_equilibrium_cure_brine_calculator() {
    equilibrium_cure_brine_calculator::init();
    auto s = equilibrium_cure_brine_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    equilibrium_cure_brine_calculator::shutdown();
    return testing::ok();
}

const int _reg_equilibrium_cure_brine_calculator = testing::register_test(
    "equilibrium_cure_brine_calculator",
    "1787_equilibrium_cure_brine_calculator: stub status check",
    &run_equilibrium_cure_brine_calculator);

}
