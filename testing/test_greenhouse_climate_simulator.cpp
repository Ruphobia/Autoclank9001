#include "test_runner.hpp"
#include "../modules/1616_greenhouse_climate_simulator/greenhouse_climate_simulator.hpp"

namespace {

testing::TestOutcome run_greenhouse_climate_simulator() {
    greenhouse_climate_simulator::init();
    auto s = greenhouse_climate_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    greenhouse_climate_simulator::shutdown();
    return testing::ok();
}

const int _reg_greenhouse_climate_simulator = testing::register_test(
    "greenhouse_climate_simulator",
    "1616_greenhouse_climate_simulator: stub status check",
    &run_greenhouse_climate_simulator);

}
