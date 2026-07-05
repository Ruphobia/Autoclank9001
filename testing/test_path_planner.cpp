#include "test_runner.hpp"
#include "../modules/1248_path_planner/path_planner.hpp"

namespace {

testing::TestOutcome run_path_planner() {
    path_planner::init();
    auto s = path_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    path_planner::shutdown();
    return testing::ok();
}

const int _reg_path_planner = testing::register_test(
    "path_planner",
    "1248_path_planner: stub status check",
    &run_path_planner);

}
