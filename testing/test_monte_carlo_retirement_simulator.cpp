#include "test_runner.hpp"
#include "../modules/566_monte_carlo_retirement_simulator/monte_carlo_retirement_simulator.hpp"

namespace {

testing::TestOutcome run_monte_carlo_retirement_simulator() {
    monte_carlo_retirement_simulator::init();
    auto s = monte_carlo_retirement_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    monte_carlo_retirement_simulator::shutdown();
    return testing::ok();
}

const int _reg_monte_carlo_retirement_simulator = testing::register_test(
    "monte_carlo_retirement_simulator",
    "566_monte_carlo_retirement_simulator: stub status check",
    &run_monte_carlo_retirement_simulator);

}
