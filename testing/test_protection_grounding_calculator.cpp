#include "test_runner.hpp"
#include "../modules/1293_protection_grounding_calculator/protection_grounding_calculator.hpp"

namespace {

testing::TestOutcome run_protection_grounding_calculator() {
    protection_grounding_calculator::init();
    auto s = protection_grounding_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    protection_grounding_calculator::shutdown();
    return testing::ok();
}

const int _reg_protection_grounding_calculator = testing::register_test(
    "protection_grounding_calculator",
    "1293_protection_grounding_calculator: stub status check",
    &run_protection_grounding_calculator);

}
