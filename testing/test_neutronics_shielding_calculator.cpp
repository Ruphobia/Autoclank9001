#include "test_runner.hpp"
#include "../modules/1101_neutronics_shielding_calculator/neutronics_shielding_calculator.hpp"

namespace {

testing::TestOutcome run_neutronics_shielding_calculator() {
    neutronics_shielding_calculator::init();
    auto s = neutronics_shielding_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    neutronics_shielding_calculator::shutdown();
    return testing::ok();
}

const int _reg_neutronics_shielding_calculator = testing::register_test(
    "neutronics_shielding_calculator",
    "1101_neutronics_shielding_calculator: stub status check",
    &run_neutronics_shielding_calculator);

}
