#include "test_runner.hpp"
#include "../modules/269_network_interface_configurator/network_interface_configurator.hpp"

namespace {

testing::TestOutcome run_network_interface_configurator() {
    network_interface_configurator::init();
    auto s = network_interface_configurator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    network_interface_configurator::shutdown();
    return testing::ok();
}

const int _reg_network_interface_configurator = testing::register_test(
    "network_interface_configurator",
    "269_network_interface_configurator: stub status check",
    &run_network_interface_configurator);

}
