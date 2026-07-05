#include "test_runner.hpp"
#include "../modules/876_polarization_calculator/polarization_calculator.hpp"

namespace {

testing::TestOutcome run_polarization_calculator() {
    polarization_calculator::init();
    auto s = polarization_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polarization_calculator::shutdown();
    return testing::ok();
}

const int _reg_polarization_calculator = testing::register_test(
    "polarization_calculator",
    "876_polarization_calculator: stub status check",
    &run_polarization_calculator);

}
