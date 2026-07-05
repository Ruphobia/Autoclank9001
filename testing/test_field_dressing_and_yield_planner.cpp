#include "test_runner.hpp"
#include "../modules/2055_field_dressing_and_yield_planner/field_dressing_and_yield_planner.hpp"

namespace {

testing::TestOutcome run_field_dressing_and_yield_planner() {
    field_dressing_and_yield_planner::init();
    auto s = field_dressing_and_yield_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    field_dressing_and_yield_planner::shutdown();
    return testing::ok();
}

const int _reg_field_dressing_and_yield_planner = testing::register_test(
    "field_dressing_and_yield_planner",
    "2055_field_dressing_and_yield_planner: stub status check",
    &run_field_dressing_and_yield_planner);

}
