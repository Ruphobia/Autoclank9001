#include "test_runner.hpp"
#include "../modules/2038_concrete_and_aggregate_volume_calculator/concrete_and_aggregate_volume_calculator.hpp"

namespace {

testing::TestOutcome run_concrete_and_aggregate_volume_calculator() {
    concrete_and_aggregate_volume_calculator::init();
    auto s = concrete_and_aggregate_volume_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    concrete_and_aggregate_volume_calculator::shutdown();
    return testing::ok();
}

const int _reg_concrete_and_aggregate_volume_calculator = testing::register_test(
    "concrete_and_aggregate_volume_calculator",
    "2038_concrete_and_aggregate_volume_calculator: stub status check",
    &run_concrete_and_aggregate_volume_calculator);

}
