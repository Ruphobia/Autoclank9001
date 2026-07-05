#include "test_runner.hpp"
#include "../modules/2020_motor_and_hvac_circuit_sizer/motor_and_hvac_circuit_sizer.hpp"

namespace {

testing::TestOutcome run_motor_and_hvac_circuit_sizer() {
    motor_and_hvac_circuit_sizer::init();
    auto s = motor_and_hvac_circuit_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    motor_and_hvac_circuit_sizer::shutdown();
    return testing::ok();
}

const int _reg_motor_and_hvac_circuit_sizer = testing::register_test(
    "motor_and_hvac_circuit_sizer",
    "2020_motor_and_hvac_circuit_sizer: stub status check",
    &run_motor_and_hvac_circuit_sizer);

}
