#include "test_runner.hpp"
#include "../modules/1005_density_and_mass_calculator/density_and_mass_calculator.hpp"

namespace {

testing::TestOutcome run_density_and_mass_calculator() {
    density_and_mass_calculator::init();
    auto s = density_and_mass_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    density_and_mass_calculator::shutdown();
    return testing::ok();
}

const int _reg_density_and_mass_calculator = testing::register_test(
    "density_and_mass_calculator",
    "1005_density_and_mass_calculator: stub status check",
    &run_density_and_mass_calculator);

}
