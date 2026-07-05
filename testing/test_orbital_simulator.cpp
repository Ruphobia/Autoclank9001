#include "test_runner.hpp"
#include "../modules/1485_orbital_simulator/orbital_simulator.hpp"

namespace {

testing::TestOutcome run_orbital_simulator() {
    orbital_simulator::init();
    auto s = orbital_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    orbital_simulator::shutdown();
    return testing::ok();
}

const int _reg_orbital_simulator = testing::register_test(
    "orbital_simulator",
    "1485_orbital_simulator: stub status check",
    &run_orbital_simulator);

}
