#include "test_runner.hpp"
#include "../modules/1339_gel_simulator/gel_simulator.hpp"

namespace {

testing::TestOutcome run_gel_simulator() {
    gel_simulator::init();
    auto s = gel_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gel_simulator::shutdown();
    return testing::ok();
}

const int _reg_gel_simulator = testing::register_test(
    "gel_simulator",
    "1339_gel_simulator: stub status check",
    &run_gel_simulator);

}
