#include "test_runner.hpp"
#include "../modules/491_network_flow_solver/network_flow_solver.hpp"

namespace {

testing::TestOutcome run_network_flow_solver() {
    network_flow_solver::init();
    auto s = network_flow_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    network_flow_solver::shutdown();
    return testing::ok();
}

const int _reg_network_flow_solver = testing::register_test(
    "network_flow_solver",
    "491_network_flow_solver: stub status check",
    &run_network_flow_solver);

}
