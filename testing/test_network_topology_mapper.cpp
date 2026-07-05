#include "test_runner.hpp"
#include "../modules/365_network_topology_mapper/network_topology_mapper.hpp"

namespace {

testing::TestOutcome run_network_topology_mapper() {
    network_topology_mapper::init();
    auto s = network_topology_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    network_topology_mapper::shutdown();
    return testing::ok();
}

const int _reg_network_topology_mapper = testing::register_test(
    "network_topology_mapper",
    "365_network_topology_mapper: stub status check",
    &run_network_topology_mapper);

}
