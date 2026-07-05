#include "test_runner.hpp"
#include "../modules/1617_rainwater_catchment_calculator/rainwater_catchment_calculator.hpp"

namespace {

testing::TestOutcome run_rainwater_catchment_calculator() {
    rainwater_catchment_calculator::init();
    auto s = rainwater_catchment_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rainwater_catchment_calculator::shutdown();
    return testing::ok();
}

const int _reg_rainwater_catchment_calculator = testing::register_test(
    "rainwater_catchment_calculator",
    "1617_rainwater_catchment_calculator: stub status check",
    &run_rainwater_catchment_calculator);

}
