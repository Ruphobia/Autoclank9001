#include "test_runner.hpp"
#include "../modules/1051_earthwork_calculator/earthwork_calculator.hpp"

namespace {

testing::TestOutcome run_earthwork_calculator() {
    earthwork_calculator::init();
    auto s = earthwork_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    earthwork_calculator::shutdown();
    return testing::ok();
}

const int _reg_earthwork_calculator = testing::register_test(
    "earthwork_calculator",
    "1051_earthwork_calculator: stub status check",
    &run_earthwork_calculator);

}
