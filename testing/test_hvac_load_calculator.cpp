#include "test_runner.hpp"
#include "../modules/1040_hvac_load_calculator/hvac_load_calculator.hpp"

namespace {

testing::TestOutcome run_hvac_load_calculator() {
    hvac_load_calculator::init();
    auto s = hvac_load_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hvac_load_calculator::shutdown();
    return testing::ok();
}

const int _reg_hvac_load_calculator = testing::register_test(
    "hvac_load_calculator",
    "1040_hvac_load_calculator: stub status check",
    &run_hvac_load_calculator);

}
