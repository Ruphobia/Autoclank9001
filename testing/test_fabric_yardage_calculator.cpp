#include "test_runner.hpp"
#include "../modules/1767_fabric_yardage_calculator/fabric_yardage_calculator.hpp"

namespace {

testing::TestOutcome run_fabric_yardage_calculator() {
    fabric_yardage_calculator::init();
    auto s = fabric_yardage_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fabric_yardage_calculator::shutdown();
    return testing::ok();
}

const int _reg_fabric_yardage_calculator = testing::register_test(
    "fabric_yardage_calculator",
    "1767_fabric_yardage_calculator: stub status check",
    &run_fabric_yardage_calculator);

}
