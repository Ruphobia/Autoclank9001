#include "test_runner.hpp"
#include "../modules/2110_fuel_range_and_provisioning_calculator/fuel_range_and_provisioning_calculator.hpp"

namespace {

testing::TestOutcome run_fuel_range_and_provisioning_calculator() {
    fuel_range_and_provisioning_calculator::init();
    auto s = fuel_range_and_provisioning_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fuel_range_and_provisioning_calculator::shutdown();
    return testing::ok();
}

const int _reg_fuel_range_and_provisioning_calculator = testing::register_test(
    "fuel_range_and_provisioning_calculator",
    "2110_fuel_range_and_provisioning_calculator: stub status check",
    &run_fuel_range_and_provisioning_calculator);

}
