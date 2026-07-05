#include "test_runner.hpp"
#include "../modules/1768_drape_simulator/drape_simulator.hpp"

namespace {

testing::TestOutcome run_drape_simulator() {
    drape_simulator::init();
    auto s = drape_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drape_simulator::shutdown();
    return testing::ok();
}

const int _reg_drape_simulator = testing::register_test(
    "drape_simulator",
    "1768_drape_simulator: stub status check",
    &run_drape_simulator);

}
