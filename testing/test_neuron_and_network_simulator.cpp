#include "test_runner.hpp"
#include "../modules/1398_neuron_and_network_simulator/neuron_and_network_simulator.hpp"

namespace {

testing::TestOutcome run_neuron_and_network_simulator() {
    neuron_and_network_simulator::init();
    auto s = neuron_and_network_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    neuron_and_network_simulator::shutdown();
    return testing::ok();
}

const int _reg_neuron_and_network_simulator = testing::register_test(
    "neuron_and_network_simulator",
    "1398_neuron_and_network_simulator: stub status check",
    &run_neuron_and_network_simulator);

}
