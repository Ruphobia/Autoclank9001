#include "test_runner.hpp"
#include "../modules/1813_cheese_make_planner/cheese_make_planner.hpp"

namespace {

testing::TestOutcome run_cheese_make_planner() {
    cheese_make_planner::init();
    auto s = cheese_make_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cheese_make_planner::shutdown();
    return testing::ok();
}

const int _reg_cheese_make_planner = testing::register_test(
    "cheese_make_planner",
    "1813_cheese_make_planner: stub status check",
    &run_cheese_make_planner);

}
