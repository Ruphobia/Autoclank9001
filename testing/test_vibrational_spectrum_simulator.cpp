#include "test_runner.hpp"
#include "../modules/1303_vibrational_spectrum_simulator/vibrational_spectrum_simulator.hpp"

namespace {

testing::TestOutcome run_vibrational_spectrum_simulator() {
    vibrational_spectrum_simulator::init();
    auto s = vibrational_spectrum_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vibrational_spectrum_simulator::shutdown();
    return testing::ok();
}

const int _reg_vibrational_spectrum_simulator = testing::register_test(
    "vibrational_spectrum_simulator",
    "1303_vibrational_spectrum_simulator: stub status check",
    &run_vibrational_spectrum_simulator);

}
