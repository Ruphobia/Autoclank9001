#include "test_runner.hpp"
#include "../modules/1590_cipw_norm_calculator/cipw_norm_calculator.hpp"

namespace {

testing::TestOutcome run_cipw_norm_calculator() {
    cipw_norm_calculator::init();
    auto s = cipw_norm_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cipw_norm_calculator::shutdown();
    return testing::ok();
}

const int _reg_cipw_norm_calculator = testing::register_test(
    "cipw_norm_calculator",
    "1590_cipw_norm_calculator: stub status check",
    &run_cipw_norm_calculator);

}
