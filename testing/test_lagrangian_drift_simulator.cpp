#include "test_runner.hpp"
#include "../modules/1557_lagrangian_drift_simulator/lagrangian_drift_simulator.hpp"

namespace {

testing::TestOutcome run_lagrangian_drift_simulator() {
    lagrangian_drift_simulator::init();
    auto s = lagrangian_drift_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lagrangian_drift_simulator::shutdown();
    return testing::ok();
}

const int _reg_lagrangian_drift_simulator = testing::register_test(
    "lagrangian_drift_simulator",
    "1557_lagrangian_drift_simulator: stub status check",
    &run_lagrangian_drift_simulator);

}
