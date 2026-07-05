#include "test_runner.hpp"
#include "../modules/484_discrete_event_simulator/discrete_event_simulator.hpp"

namespace {

testing::TestOutcome run_discrete_event_simulator() {
    discrete_event_simulator::init();
    auto s = discrete_event_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    discrete_event_simulator::shutdown();
    return testing::ok();
}

const int _reg_discrete_event_simulator = testing::register_test(
    "discrete_event_simulator",
    "484_discrete_event_simulator: stub status check",
    &run_discrete_event_simulator);

}
