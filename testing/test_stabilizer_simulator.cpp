#include "test_runner.hpp"
#include "../modules/334_stabilizer_simulator/stabilizer_simulator.hpp"

namespace {

testing::TestOutcome run_stabilizer_simulator() {
    stabilizer_simulator::init();
    auto s = stabilizer_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stabilizer_simulator::shutdown();
    return testing::ok();
}

const int _reg_stabilizer_simulator = testing::register_test(
    "stabilizer_simulator",
    "334_stabilizer_simulator: stub status check",
    &run_stabilizer_simulator);

}
