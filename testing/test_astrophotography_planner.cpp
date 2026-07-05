#include "test_runner.hpp"
#include "../modules/1669_astrophotography_planner/astrophotography_planner.hpp"

namespace {

testing::TestOutcome run_astrophotography_planner() {
    astrophotography_planner::init();
    auto s = astrophotography_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    astrophotography_planner::shutdown();
    return testing::ok();
}

const int _reg_astrophotography_planner = testing::register_test(
    "astrophotography_planner",
    "1669_astrophotography_planner: stub status check",
    &run_astrophotography_planner);

}
