#include "test_runner.hpp"
#include "../modules/335_tensor_network_simulator/tensor_network_simulator.hpp"

namespace {

testing::TestOutcome run_tensor_network_simulator() {
    tensor_network_simulator::init();
    auto s = tensor_network_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tensor_network_simulator::shutdown();
    return testing::ok();
}

const int _reg_tensor_network_simulator = testing::register_test(
    "tensor_network_simulator",
    "335_tensor_network_simulator: stub status check",
    &run_tensor_network_simulator);

}
