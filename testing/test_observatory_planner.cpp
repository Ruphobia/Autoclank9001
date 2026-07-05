#include "test_runner.hpp"
#include "../modules/1489_observatory_planner/observatory_planner.hpp"

namespace {

testing::TestOutcome run_observatory_planner() {
    observatory_planner::init();
    auto s = observatory_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    observatory_planner::shutdown();
    return testing::ok();
}

const int _reg_observatory_planner = testing::register_test(
    "observatory_planner",
    "1489_observatory_planner: stub status check",
    &run_observatory_planner);

}
