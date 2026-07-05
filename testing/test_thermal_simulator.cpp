#include "test_runner.hpp"
#include "../modules/854_thermal_simulator/thermal_simulator.hpp"

namespace {

testing::TestOutcome run_thermal_simulator() {
    thermal_simulator::init();
    auto s = thermal_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    thermal_simulator::shutdown();
    return testing::ok();
}

const int _reg_thermal_simulator = testing::register_test(
    "thermal_simulator",
    "854_thermal_simulator: stub status check",
    &run_thermal_simulator);

}
