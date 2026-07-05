#include "test_runner.hpp"
#include "../modules/1909_pricing_and_session_planner/pricing_and_session_planner.hpp"

namespace {

testing::TestOutcome run_pricing_and_session_planner() {
    pricing_and_session_planner::init();
    auto s = pricing_and_session_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pricing_and_session_planner::shutdown();
    return testing::ok();
}

const int _reg_pricing_and_session_planner = testing::register_test(
    "pricing_and_session_planner",
    "1909_pricing_and_session_planner: stub status check",
    &run_pricing_and_session_planner);

}
