#include "test_runner.hpp"
#include "../modules/1544_teos_10_calculator/teos_10_calculator.hpp"

namespace {

testing::TestOutcome run_teos_10_calculator() {
    teos_10_calculator::init();
    auto s = teos_10_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    teos_10_calculator::shutdown();
    return testing::ok();
}

const int _reg_teos_10_calculator = testing::register_test(
    "teos_10_calculator",
    "1544_teos_10_calculator: stub status check",
    &run_teos_10_calculator);

}
