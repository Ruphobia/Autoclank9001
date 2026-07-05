#include "test_runner.hpp"
#include "../modules/1062_stormwater_network_router/stormwater_network_router.hpp"

namespace {

testing::TestOutcome run_stormwater_network_router() {
    stormwater_network_router::init();
    auto s = stormwater_network_router::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stormwater_network_router::shutdown();
    return testing::ok();
}

const int _reg_stormwater_network_router = testing::register_test(
    "stormwater_network_router",
    "1062_stormwater_network_router: stub status check",
    &run_stormwater_network_router);

}
