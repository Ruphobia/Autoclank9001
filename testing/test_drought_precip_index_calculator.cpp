#include "test_runner.hpp"
#include "../modules/1468_drought_precip_index_calculator/drought_precip_index_calculator.hpp"

namespace {

testing::TestOutcome run_drought_precip_index_calculator() {
    drought_precip_index_calculator::init();
    auto s = drought_precip_index_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drought_precip_index_calculator::shutdown();
    return testing::ok();
}

const int _reg_drought_precip_index_calculator = testing::register_test(
    "drought_precip_index_calculator",
    "1468_drought_precip_index_calculator: stub status check",
    &run_drought_precip_index_calculator);

}
