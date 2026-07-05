#include "test_runner.hpp"
#include "../modules/1299_stoichiometry_calculator/stoichiometry_calculator.hpp"

namespace {

testing::TestOutcome run_stoichiometry_calculator() {
    stoichiometry_calculator::init();
    auto s = stoichiometry_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stoichiometry_calculator::shutdown();
    return testing::ok();
}

const int _reg_stoichiometry_calculator = testing::register_test(
    "stoichiometry_calculator",
    "1299_stoichiometry_calculator: stub status check",
    &run_stoichiometry_calculator);

}
