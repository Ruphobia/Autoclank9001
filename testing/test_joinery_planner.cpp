#include "test_runner.hpp"
#include "../modules/979_joinery_planner/joinery_planner.hpp"

namespace {

testing::TestOutcome run_joinery_planner() {
    joinery_planner::init();
    auto s = joinery_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    joinery_planner::shutdown();
    return testing::ok();
}

const int _reg_joinery_planner = testing::register_test(
    "joinery_planner",
    "979_joinery_planner: stub status check",
    &run_joinery_planner);

}
