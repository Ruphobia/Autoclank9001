#include "test_runner.hpp"
#include "../modules/2104_route_and_passage_planner/route_and_passage_planner.hpp"

namespace {

testing::TestOutcome run_route_and_passage_planner() {
    route_and_passage_planner::init();
    auto s = route_and_passage_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    route_and_passage_planner::shutdown();
    return testing::ok();
}

const int _reg_route_and_passage_planner = testing::register_test(
    "route_and_passage_planner",
    "2104_route_and_passage_planner: stub status check",
    &run_route_and_passage_planner);

}
