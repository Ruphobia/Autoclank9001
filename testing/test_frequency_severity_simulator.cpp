#include "test_runner.hpp"
#include "../modules/545_frequency_severity_simulator/frequency_severity_simulator.hpp"

namespace {

testing::TestOutcome run_frequency_severity_simulator() {
    frequency_severity_simulator::init();
    auto s = frequency_severity_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frequency_severity_simulator::shutdown();
    return testing::ok();
}

const int _reg_frequency_severity_simulator = testing::register_test(
    "frequency_severity_simulator",
    "545_frequency_severity_simulator: stub status check",
    &run_frequency_severity_simulator);

}
