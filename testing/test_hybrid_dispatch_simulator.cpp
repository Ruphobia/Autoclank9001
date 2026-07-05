#include "test_runner.hpp"
#include "../modules/1284_hybrid_dispatch_simulator/hybrid_dispatch_simulator.hpp"

namespace {

testing::TestOutcome run_hybrid_dispatch_simulator() {
    hybrid_dispatch_simulator::init();
    auto s = hybrid_dispatch_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hybrid_dispatch_simulator::shutdown();
    return testing::ok();
}

const int _reg_hybrid_dispatch_simulator = testing::register_test(
    "hybrid_dispatch_simulator",
    "1284_hybrid_dispatch_simulator: stub status check",
    &run_hybrid_dispatch_simulator);

}
