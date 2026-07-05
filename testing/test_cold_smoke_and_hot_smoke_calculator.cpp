#include "test_runner.hpp"
#include "../modules/1814_cold_smoke_and_hot_smoke_calculator/cold_smoke_and_hot_smoke_calculator.hpp"

namespace {

testing::TestOutcome run_cold_smoke_and_hot_smoke_calculator() {
    cold_smoke_and_hot_smoke_calculator::init();
    auto s = cold_smoke_and_hot_smoke_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cold_smoke_and_hot_smoke_calculator::shutdown();
    return testing::ok();
}

const int _reg_cold_smoke_and_hot_smoke_calculator = testing::register_test(
    "cold_smoke_and_hot_smoke_calculator",
    "1814_cold_smoke_and_hot_smoke_calculator: stub status check",
    &run_cold_smoke_and_hot_smoke_calculator);

}
