#include "test_runner.hpp"
#include "../modules/1174_performance_calculator/performance_calculator.hpp"

namespace {

testing::TestOutcome run_performance_calculator() {
    performance_calculator::init();
    auto s = performance_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    performance_calculator::shutdown();
    return testing::ok();
}

const int _reg_performance_calculator = testing::register_test(
    "performance_calculator",
    "1174_performance_calculator: stub status check",
    &run_performance_calculator);

}
