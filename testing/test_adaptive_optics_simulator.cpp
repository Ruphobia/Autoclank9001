#include "test_runner.hpp"
#include "../modules/888_adaptive_optics_simulator/adaptive_optics_simulator.hpp"

namespace {

testing::TestOutcome run_adaptive_optics_simulator() {
    adaptive_optics_simulator::init();
    auto s = adaptive_optics_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    adaptive_optics_simulator::shutdown();
    return testing::ok();
}

const int _reg_adaptive_optics_simulator = testing::register_test(
    "adaptive_optics_simulator",
    "888_adaptive_optics_simulator: stub status check",
    &run_adaptive_optics_simulator);

}
