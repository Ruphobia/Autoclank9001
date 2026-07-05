#include "test_runner.hpp"
#include "../modules/1029_view_factor_calculator/view_factor_calculator.hpp"

namespace {

testing::TestOutcome run_view_factor_calculator() {
    view_factor_calculator::init();
    auto s = view_factor_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    view_factor_calculator::shutdown();
    return testing::ok();
}

const int _reg_view_factor_calculator = testing::register_test(
    "view_factor_calculator",
    "1029_view_factor_calculator: stub status check",
    &run_view_factor_calculator);

}
