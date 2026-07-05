#include "test_runner.hpp"
#include "../modules/2087_season_monte_carlo_simulator/season_monte_carlo_simulator.hpp"

namespace {

testing::TestOutcome run_season_monte_carlo_simulator() {
    season_monte_carlo_simulator::init();
    auto s = season_monte_carlo_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    season_monte_carlo_simulator::shutdown();
    return testing::ok();
}

const int _reg_season_monte_carlo_simulator = testing::register_test(
    "season_monte_carlo_simulator",
    "2087_season_monte_carlo_simulator: stub status check",
    &run_season_monte_carlo_simulator);

}
