#include "test_runner.hpp"
#include "../modules/1032_column_calculator/column_calculator.hpp"

namespace {

testing::TestOutcome run_column_calculator() {
    column_calculator::init();
    auto s = column_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    column_calculator::shutdown();
    return testing::ok();
}

const int _reg_column_calculator = testing::register_test(
    "column_calculator",
    "1032_column_calculator: stub status check",
    &run_column_calculator);

}
