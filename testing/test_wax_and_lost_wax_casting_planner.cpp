#include "test_runner.hpp"
#include "../modules/1858_wax_and_lost_wax_casting_planner/wax_and_lost_wax_casting_planner.hpp"

namespace {

testing::TestOutcome run_wax_and_lost_wax_casting_planner() {
    wax_and_lost_wax_casting_planner::init();
    auto s = wax_and_lost_wax_casting_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wax_and_lost_wax_casting_planner::shutdown();
    return testing::ok();
}

const int _reg_wax_and_lost_wax_casting_planner = testing::register_test(
    "wax_and_lost_wax_casting_planner",
    "1858_wax_and_lost_wax_casting_planner: stub status check",
    &run_wax_and_lost_wax_casting_planner);

}
