#include "test_runner.hpp"
#include "../modules/1236_valve_clearance_shim_calculator/valve_clearance_shim_calculator.hpp"

namespace {

testing::TestOutcome run_valve_clearance_shim_calculator() {
    valve_clearance_shim_calculator::init();
    auto s = valve_clearance_shim_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    valve_clearance_shim_calculator::shutdown();
    return testing::ok();
}

const int _reg_valve_clearance_shim_calculator = testing::register_test(
    "valve_clearance_shim_calculator",
    "1236_valve_clearance_shim_calculator: stub status check",
    &run_valve_clearance_shim_calculator);

}
