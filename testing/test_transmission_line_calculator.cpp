#include "test_runner.hpp"
#include "../modules/1188_transmission_line_calculator/transmission_line_calculator.hpp"

namespace {

testing::TestOutcome run_transmission_line_calculator() {
    transmission_line_calculator::init();
    auto s = transmission_line_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transmission_line_calculator::shutdown();
    return testing::ok();
}

const int _reg_transmission_line_calculator = testing::register_test(
    "transmission_line_calculator",
    "1188_transmission_line_calculator: stub status check",
    &run_transmission_line_calculator);

}
