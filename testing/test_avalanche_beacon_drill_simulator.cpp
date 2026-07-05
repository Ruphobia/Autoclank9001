#include "test_runner.hpp"
#include "../modules/2069_avalanche_beacon_drill_simulator/avalanche_beacon_drill_simulator.hpp"

namespace {

testing::TestOutcome run_avalanche_beacon_drill_simulator() {
    avalanche_beacon_drill_simulator::init();
    auto s = avalanche_beacon_drill_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    avalanche_beacon_drill_simulator::shutdown();
    return testing::ok();
}

const int _reg_avalanche_beacon_drill_simulator = testing::register_test(
    "avalanche_beacon_drill_simulator",
    "2069_avalanche_beacon_drill_simulator: stub status check",
    &run_avalanche_beacon_drill_simulator);

}
