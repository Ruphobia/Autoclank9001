#include "test_runner.hpp"
#include "../modules/902_rigid_body_simulator/rigid_body_simulator.hpp"

namespace {

testing::TestOutcome run_rigid_body_simulator() {
    rigid_body_simulator::init();
    auto s = rigid_body_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rigid_body_simulator::shutdown();
    return testing::ok();
}

const int _reg_rigid_body_simulator = testing::register_test(
    "rigid_body_simulator",
    "902_rigid_body_simulator: stub status check",
    &run_rigid_body_simulator);

}
