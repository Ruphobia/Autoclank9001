#include "test_runner.hpp"
#include "../modules/1797_modernist_hydrocolloid_calculator/modernist_hydrocolloid_calculator.hpp"

namespace {

testing::TestOutcome run_modernist_hydrocolloid_calculator() {
    modernist_hydrocolloid_calculator::init();
    auto s = modernist_hydrocolloid_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    modernist_hydrocolloid_calculator::shutdown();
    return testing::ok();
}

const int _reg_modernist_hydrocolloid_calculator = testing::register_test(
    "modernist_hydrocolloid_calculator",
    "1797_modernist_hydrocolloid_calculator: stub status check",
    &run_modernist_hydrocolloid_calculator);

}
