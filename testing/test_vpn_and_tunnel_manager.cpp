#include "test_runner.hpp"
#include "../modules/275_vpn_and_tunnel_manager/vpn_and_tunnel_manager.hpp"

namespace {

testing::TestOutcome run_vpn_and_tunnel_manager() {
    vpn_and_tunnel_manager::init();
    auto s = vpn_and_tunnel_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vpn_and_tunnel_manager::shutdown();
    return testing::ok();
}

const int _reg_vpn_and_tunnel_manager = testing::register_test(
    "vpn_and_tunnel_manager",
    "275_vpn_and_tunnel_manager: stub status check",
    &run_vpn_and_tunnel_manager);

}
